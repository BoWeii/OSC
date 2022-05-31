#ifndef __TMPFS_H_
#define __TMPFS_H_

struct filesystem *tmfps_create();
struct vnode* vnode_create(const char *name, unsigned int flags);

extern struct file_operations tmpfs_f_ops;
extern struct vnode_operations tmpfs_v_ops;
#endif