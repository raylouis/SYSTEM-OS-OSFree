/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2001   Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if defined(fsys_vstafs) || defined(FSYS_VSTAFS)

#include "shared.h"
#include "filesys.h"
#include "vstafs.h"

#include "fsys.h"
#include "misc.h"
#include "fsd.h"

char fs_name[] = "vstafs";

static void get_file_info (int sector);
static struct dir_entry *vstafs_readdir (long sector);
static struct dir_entry *vstafs_nextdir (void);


#define FIRST_SECTOR    ((struct first_sector *) FSYS_BUF)
#define FILE_INFO       ((struct fs_file *) ((int)(FIRST_SECTOR + 8192)))
#define DIRECTORY_BUF   ((struct dir_entry *) ((int)(FILE_INFO + 512)))

#define ROOT_SECTOR     1

/*
 * In f_sector we store the sector number in which the information about
 * the found file is.
 */
extern int *pfilepos;
static int f_sector;

int
vstafs_mount (void)
{
  int retval = 1;

  if ( /* ((*pcurrent_drive & 0x80) || (*pcurrent_slice != 0))
       && *pcurrent_slice != PC_SLICE_TYPE_VSTAFS)
      || */  ! (*pdevread) (0, 0, BLOCK_SIZE, (char *) FSYS_BUF)
      ||  FIRST_SECTOR->fs_magic != 0xDEADFACE)
    retval = 0;

  return retval;
}

static void
get_file_info (int sector)
{
  (*pdevread) (sector, 0, BLOCK_SIZE, (char *) FILE_INFO);
}

static int curr_ext, current_direntry, current_blockpos;
static struct alloc *a;

static struct dir_entry *
vstafs_readdir (long sector)
{
  /*
   * Get some information from the current directory
   */
  get_file_info (sector);
  if (FILE_INFO->type != 2)
    {
      *perrnum = ERR_FILE_NOT_FOUND;
      return 0;
    }

  a = (struct alloc *)FILE_INFO->blocks;
  curr_ext = 0;
  (*pdevread) (a[curr_ext].a_start, 0, 512, (char *) DIRECTORY_BUF);
  current_direntry = 11;
  current_blockpos = 0;

  return &DIRECTORY_BUF[10];
}

static struct dir_entry *
vstafs_nextdir (void)
{
  if (current_direntry > 15)
    {
      current_direntry = 0;
      if (++current_blockpos > (a[curr_ext].a_len - 1))
        {
          current_blockpos = 0;
          curr_ext++;
        }

      if (curr_ext < FILE_INFO->extents)
        {
          (*pdevread) (a[curr_ext].a_start + current_blockpos, 0,
                   512, (char *) DIRECTORY_BUF);
        }
      else
        {
          /* *perrnum =ERR_FILE_NOT_FOUND; */
          return 0;
        }
    }

  return &DIRECTORY_BUF[current_direntry++];
}

int
vstafs_dir (char *dirname)
{
  char *fn, ch;
  struct dir_entry *d;
  /* int l, i, s; */

  /*
   * Read in the entries of the current directory.
   */
  f_sector = ROOT_SECTOR;
  do
    {
      if (! (d = vstafs_readdir (f_sector)))
        {
          return 0;
        }

      /*
       * Find the file in the path
       */
      while (*dirname == '/') dirname++;
      fn = dirname;
      while ((ch = *fn) && ch != '/' && ! (*pgrub_isspace) (ch)) fn++;
      *fn = 0;

      do
        {
          if (d->name[0] == 0 || d->name[0] & 0x80)
            continue;

#ifndef STAGE1_5
          if (*pprint_possibilities && ch != '/'
              && (! *dirname || (*pgrub_strcmp) (dirname, d->name) <= 0))
            {
              if (*pprint_possibilities > 0)
                *pprint_possibilities = - *pprint_possibilities;

              (*pprintf) ("  %s", d->name);
            }
#endif
          if (! (*pgrub_strcmp) (dirname, d->name))
            {
              f_sector = d->start;
              get_file_info (f_sector);
              *pfilemax = FILE_INFO->len;
              break;
            }
        }
      while ((d =vstafs_nextdir ()));

      *(dirname = fn) = ch;
      if (! d)
        {
          //if (*pprint_possibilities < 0)
          //  {
          //    putchar ('\n');
          //    return 1;
          //  }

          *perrnum = ERR_FILE_NOT_FOUND;
          return 0;
        }
    }
  while (*dirname && ! (*pgrub_isspace) (ch));

  return 1;
}

int
vstafs_read (char *addr, int len)
{
  struct alloc *a;
  int size, ret = 0, offset, curr_len = 0;
  int curr_ext;
  char extent;
  int ext_size;
  char *curr_pos;

  get_file_info (f_sector);
  size = FILE_INFO->len-VSTAFS_START_DATA;
  a = (struct alloc *)FILE_INFO->blocks;

  if (*pfilepos > 0)
    {
      if (*pfilepos < a[0].a_len * 512 - VSTAFS_START_DATA)
        {
          offset = *pfilepos + VSTAFS_START_DATA;
          extent = 0;
          curr_len = a[0].a_len * 512 - offset - *pfilepos;
        }
      else
        {
          ext_size = a[0].a_len * 512 - VSTAFS_START_DATA;
          offset = *pfilepos - ext_size;
          extent = 1;
          do
            {
              curr_len -= ext_size;
              offset -= ext_size;
              ext_size = a[extent+1].a_len * 512;
            }
          while (extent < FILE_INFO->extents && offset>ext_size);
        }
    }
  else
    {
      offset = VSTAFS_START_DATA;
      extent = 0;
      curr_len = a[0].a_len * 512 - offset;
    }

  curr_pos = addr;
  if (curr_len > len)
    curr_len = len;

  for (curr_ext=extent;
       curr_ext < FILE_INFO->extents;
       curr_len = a[curr_ext].a_len * 512, curr_pos += curr_len, curr_ext++)
    {
      ret += curr_len;
      size -= curr_len;
      if (size < 0)
        {
          ret += size;
          curr_len += size;
        }

      *pdisk_read_func = *pdisk_read_hook; //vs

      (*pdevread) (a[curr_ext].a_start,offset, curr_len, curr_pos);

      *pdisk_read_func = NULL; //vs

      offset = 0;
    }

  return ret;
}

#endif /* FSYS_VSTAFS */
