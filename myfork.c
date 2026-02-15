#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

SYSCALL_DEFINE0(myfork) {
   
    pr_info("myfork: Process %d is cloning itself.\n", task_tgid_vnr(current));

    return kernel_clone(&(struct kernel_clone_args){.exit_signal = SIGCHLD});
}