/** \file xmlfs.c
 *  \brief source for mount.fuse.xml
 *         
 * This is the source file for fuse xml file system
 * \author blunderer <blunderer@blunderer.org>
 * \date 15 Apr 2009
 *
 * Copyright (C) 2009 blunderer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef XMLFS_C
#define XMLFS_C

#define FUSE_USE_VERSION 26
 
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
 
#include <roxml.h>
 

#define MAX_ENTRIES	512
#define LOG_FILE	"/var/log/xmlfs.log"
#define NODE_CONTENT	"content.data"

node_t ** opened_files = NULL;
FILE *debug_f = NULL;

#define DEBUG(...) { if(debug_f) { fprintf(debug_f,"%s::%s:%d ",__FILE__,__func__,__LINE__);fprintf(debug_f,__VA_ARGS__); fprintf(debug_f,"\n"); fflush(debug_f); } }
 
static int xmlfs_getattr(const char *path, struct stat *stbuf)
{
	int nb;
	int fsize = 1;
	int content = 0;
	node_t *n = NULL;
	node_t *root = fuse_get_context()->private_data;
	char newpath[512];
	char *ptr;

	strcpy(newpath, path);
	if(ptr = strstr(newpath, NODE_CONTENT))	{
		*ptr = 0;
		content = 1;
	}

	DEBUG("trying '%s'",newpath);
	node_t **ans = roxml_exec_path(root, newpath, &nb);

	if(ans) {
		n = ans[0];
		if((roxml_is_arg(n))||(content))	{
			fsize = roxml_get_content(n, NULL);
			stbuf->st_mode = S_IFREG;
			DEBUG("file stat : %d",fsize)
		} else {
			fsize = 1;
			stbuf->st_mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
		}
		

		stbuf->st_ino = n->pos;
		stbuf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
		stbuf->st_nlink = 0;
		stbuf->st_uid = 0;
		stbuf->st_gid = 0;
		stbuf->st_rdev = 0;
		stbuf->st_size = fsize;
		stbuf->st_blksize = 1;
		stbuf->st_blocks = fsize;
		stbuf->st_atime = 0;
		stbuf->st_mtime = 0;
		stbuf->st_ctime = 0;

		return 0;
	}
	return -ENOENT;
}
 
static int xmlfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	int i;
	int nb;
	node_t *n = NULL;
	node_t *root = fuse_get_context()->private_data;
	node_t **ans = roxml_exec_path(root, (char*)path, &nb);

	if(ans)	{
		n = ans[0];
		free(ans);

		nb = roxml_get_son_nb(n);
		DEBUG("%d dirs", nb)
		for(i = 0; i < nb; i++)	{
			int idx;
			char fname[512] = "";
			node_t *tmp = roxml_get_son_nth(n, i);
			char *name = roxml_get_name(tmp);
			idx = roxml_get_node_index(tmp);
			if(idx != -1)	{
				sprintf(fname,"%s[%d]",name,idx);
				filler(buf, fname, NULL, 0);
			} else	{
				filler(buf, name, NULL, 0);
			}
			free(name);
		}
		nb = roxml_get_nb_attr(n);
		DEBUG("%d files", nb)
		for(i = 0; i < nb; i++)	{
			char fname[512] = "@";
			char *name = roxml_get_attr_nth(n, i);
			strcat(fname, name);
			filler(buf, fname, NULL, 0);
			free(name);
		}
		if(roxml_get_content(n, NULL) > 0)	{
			filler(buf, NODE_CONTENT, NULL, 0);
		}

		return 0;
	}
	return -ENOENT;
}
 
static int xmlfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	return -ENOENT;
}
 
static int xmlfs_open(const char *path, struct fuse_file_info *fi)
{
	int i = 0;
	int nb;
	node_t *root = fuse_get_context()->private_data;
	char newpath[512];
	char *ptr;

	strcpy(newpath, path);
	if(ptr = strstr(newpath, NODE_CONTENT))	{
		*ptr = 0;
	}
	node_t **ans = roxml_exec_path(root, (char*)newpath, &nb);

	if(ans)	{
		node_t *n = ans[0];
		free(ans);
		while((i < MAX_ENTRIES)&&(opened_files[i]))	{ i++; } 
		if(i < MAX_ENTRIES)	{
			fi->fh = i;
			opened_files[i] = n;

			return 0;
		}
		return -ENOENT;
	}
	return -ENOENT;
}

static int xmlfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int bsize;
	int bytes;
	node_t *n;
	char *content;
	if(size == 0) { return 0; }

	n = opened_files[fi->fh];

	if (!n) { return -ENOENT; }

	bsize = roxml_get_content(n, NULL);
	content = (char*)malloc(bsize*sizeof(char*));
	bsize = roxml_get_content(n, content);

	if((size + offset) >= bsize)	{
		bytes = bsize - offset;
	} else {
		bytes = size;
	}
	memcpy(buf, content+offset, bytes); 

	free(content);

	return bytes;
}

static int xmlfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	return -ENOENT;
}

static int xmlfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
	return -ENOENT;
}

static int xmlfs_release(const char *path, struct fuse_file_info *fi)
{
	if(fi->fh < 0) { return -ENOENT; }
	if(fi->fh > MAX_ENTRIES) { return -ENOENT; }

	DEBUG("closing file %d",fi->fh)
	opened_files[fi->fh] = NULL;
	return 0;
}

void xmlfs_cleanup(void *data)
{
	DEBUG("freeing file table")
	free(opened_files);
}

int xmlfs_flush(const char *path, struct fuse_file_info *fi)
{
	return -ENOENT;
}

int xmlfs_statfs(const char *path, struct statvfs *stats)
{
	stats->f_bsize = 0;
	stats->f_frsize = 0;
	stats->f_blocks = 0;
	stats->f_bfree = 0;
	stats->f_bavail = 0;
	stats->f_namemax = 512;
	stats->f_files = 1000000000;
	stats->f_ffree = 1000000000;
	return 0;
}

int xmlfs_truncate(const char *path, off_t size)
{
	return -ENOENT;
}

int xmlfs_ftruncate(const char *path, off_t size, struct fuse_file_info *fi)
{
	return -ENOENT;
}

int xmlfs_unlink(const char *path)
{
	return -ENOENT;
}

int xmlfs_rename(const char *from, const char *to)
{
	return -ENOENT;
}

int xmlfs_mkdir(const char *dir, mode_t ignored)
{
	return -ENOENT;
}

void *xmlfs_init(struct fuse_conn_info *conn)
{
	node_t *n = NULL;
	char *src = NULL;
	struct fuse_context *ctx = fuse_get_context();

	opened_files = (node_t**)malloc(sizeof(node_t*)*MAX_ENTRIES);

	src = (char*)ctx->private_data;
	DEBUG("mounting %s",src)

	n = roxml_load_doc(src);
	DEBUG("loaded doc to %p mount init ok", n);

	return n;
}

static struct fuse_operations xmlfs_oper = {
	.getattr = xmlfs_getattr,
	.statfs = xmlfs_statfs,
	.readdir = xmlfs_readdir,
	.mkdir = xmlfs_mkdir,
	.rmdir = xmlfs_unlink,
	.create = xmlfs_create,
	.open = xmlfs_open,
	.read = xmlfs_read,
	.write = xmlfs_write,
	.truncate = xmlfs_truncate,
	.ftruncate = xmlfs_ftruncate,
	.unlink = xmlfs_unlink,
	.rename = xmlfs_rename,
	.fsync = xmlfs_fsync,
	.release = xmlfs_release,
	.init = xmlfs_init,
	.destroy = xmlfs_cleanup
};

static int xmlfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
	char *data_arg = (char*)data;
	static int num = 0;

	switch(key) {
		case FUSE_OPT_KEY_OPT:
			if(strcmp(arg, "--debug") == 0) {
				debug_f = fopen(LOG_FILE,"w");
				fprintf(stderr,"debug mode started\n");
				return 0;
			} else if(strcmp(arg, "-oallow-other") == 0) {
				return 0;
			}
			break;
		case FUSE_OPT_KEY_NONOPT:
			num++;
			if(num == 1)	{
				strcpy(data_arg, arg);
				return 0;
			}	
			break;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	int i, j;
	char mount_src[512] = "";
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	if (argc < 2) {
		return -1;
	}

	if(fuse_opt_parse(&args, mount_src, NULL, xmlfs_opt_proc) == -1) {
		return -1;
	}
	fuse_opt_add_arg(&args, "-oallow_other");

	if(strlen(mount_src) == 0)	{
		return -1;
	} else if(mount_src[0] != '/')	{
		char tmp[512];
		strcpy(tmp, mount_src);
		getcwd(mount_src, 512);
		strcat(mount_src, "/");
		strcat(mount_src, tmp);
	}

	return fuse_main(args.argc, args.argv, &xmlfs_oper, mount_src);
}

#endif /* XMLFS_C */

