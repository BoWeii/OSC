#ifndef __TMPFS_H_
#define __TMPFS_H_

#define COMPONENT_SIZE 16
struct filesystem *tmpfs_create();
struct vnode* vnode_create(const char *name, unsigned int flags);

extern struct file_operations tmpfs_f_ops;
extern struct vnode_operations tmpfs_v_ops;
#endif