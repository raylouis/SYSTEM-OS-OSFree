/* $Id: bootsec.h,v 1.1.1.1 2003/05/21 13:41:38 pasha Exp $ */

/* static char *SCCSID = "@(#)1.2  7/31/98 08:19:13 src/jfs/utils/libfs/bootsec.h, jfslib, w45.fs32, 990417.1";
 *
 *   Copyright (c) International Business Machines  Corp., 2000
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or 
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
int32 write_bootsec(
HFILE		fd,
struct BPB *	bpb,
char *		vol_label,
int32		islog);
