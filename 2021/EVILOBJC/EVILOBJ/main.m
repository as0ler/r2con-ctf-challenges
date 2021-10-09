//
//  main.m
//
//
//  Created by Murphy on 20/7/21.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
// For debugger_sysctl
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#import <dlfcn.h>
#import <mach/mach.h>

// For ioctl
#include <termios.h>
#include <sys/ioctl.h>

// For task_get_exception_ports
#include <mach/task.h>
#include <mach/mach_init.h>

typedef int (*ptrace_ptr_t)(int _request, pid_t _pid, caddr_t _addr, int _data);


//From https://www.theiphonewiki.com/wiki/Bugging_Debuggers
void debugger_ptrace()
{
    void* handle = dlopen(0, RTLD_GLOBAL | RTLD_NOW);
    ptrace_ptr_t ptrace_ptr = dlsym(handle, "ptrace");
    ptrace_ptr(31, 0, 0, 0);
    dlclose(handle);
}

void try_kill() {
    if(1 != getppid())
        {
                exit(-1);
        }
}

static bool ptrace_detect() {
  struct kinfo_proc info;
  size_t info_size = sizeof(info);
  info.kp_proc.p_flag = 0;
  int32_t opt[4] = {
    CTL_KERN,
    KERN_PROC,
    KERN_PROC_PID,
    getpid(),
  };
  if (sysctl(opt, 4, &info, &info_size, NULL, 0) == -1)
  {
      perror("FAIL");
      exit(-1);
  }
  return ((info.kp_proc.p_flag & P_TRACED) != 0);
}


// based on : https://github.com/rustymagnet3000/ios_debugger_challenge/blob/master/debugger_challenge/objc_source/debugger_checks.m
static bool debugger_exception_ports() {

    exception_mask_t       exception_masks[EXC_TYPES_COUNT];
    mach_msg_type_number_t exception_count = 0;
    mach_port_t            exception_ports[EXC_TYPES_COUNT];
    exception_behavior_t   exception_behaviors[EXC_TYPES_COUNT];
    thread_state_flavor_t  exception_flavors[EXC_TYPES_COUNT];
    
   
    kern_return_t kr = task_get_exception_ports(
                                                mach_task_self(),
                                                EXC_MASK_BREAKPOINT,
                                                exception_masks,
                                                &exception_count,
                                                exception_ports,
                                                exception_behaviors,
                                                exception_flavors
                                                );
    if (kr == KERN_SUCCESS) {
        for (mach_msg_type_number_t i = 0; i < exception_count; i++) {
            if (MACH_PORT_VALID(exception_ports[i])) {
                return YES;
            }
        }
    }
    else {
        printf("ERROR: task_get_exception_ports: %s\n", mach_error_string(kr));
        return YES;
    }
    return NO;
}


int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        appDelegateClassName = NSStringFromClass([AppDelegate class]);
    }
//    //debugger_ptrace();
//    //try_kill();
//    //if (ptrace_detect()) {
//    //    return -1;
//    //}
//
//    if (debugger_exception_ports()) {
//        return -1;
//    }
//
//#ifdef __arm64__
//        //PTRACE /w deny attach (31)
//        asm volatile (
//                      "mov x0, #31\n"
//                      "mov x1, #0\n"
//                      "mov x2, #0\n"
//                      "mov x3, #0\n"
//                      "mov x16, #26\n"
//                      "svc #128\n"
//                      );
//#endif
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}

