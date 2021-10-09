//
//  ViewController.m
//  
//
//  Created by Murphy on 20/7/21.
//
#import <CommonCrypto/CommonDigest.h>
#import "ViewController.h"
#import <objc/runtime.h>




@interface ViewController ()

@end

@implementation ViewController

@synthesize txtFlag;
NSInteger box[25] = {0x8,0x58,0x56,0x54,0x52,0x50,0x36,0x34,0x32,0x24,0x22,0x20,0x18,0x16,0x14,0x10,0x12,0x22,0x24,0x26,0x28,0x40,042,0x44,0x60};
NSArray *hashes;

- (void)viewDidLoad {
    [super viewDidLoad];
    antiRooting();
    yetAnotherantiRooting();
    if ([ViewController f]) {
        custom_exit();
    }
    if ([ViewController g]) {
        custom_exit();
    }
    // Do any additional setup after loading the view.
    UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc]
            initWithTarget:self
                    action:@selector(tap:)];
        tapRecognizer.delegate = self;
        [self.view addGestureRecognizer:tapRecognizer];
    hashes = [NSArray arrayWithObjects:@"690382ddccb8abc7367a136262e1978f",@"47e2e8c3fdb7739e740b95345a803cac", @"c12e01f2a13ff5587e1e9e4aedb8242d", @"fd18772cbac19277b20dcccc1b90efb9", @"3691308f2a4c2f6983f2880d32e29c84", nil];
}

- (void)tap:(id)sender
{
    yetAnotherantiRooting();
    [[self view] endEditing:YES];
}


- (BOOL)checkFlag:(NSString *)flag {
    
    @try {
        if (flag.length != 44) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        if (![flag hasPrefix:@"r2con{"]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        if (![flag hasSuffix:@"4PPs}"]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
       
        if(![@"OhyK" isEqualToString:[self r:[flag substringWithRange:NSMakeRange(15, 4)]]]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        
        if([flag characterAtIndex:4] == [flag characterAtIndex:8]){
            NSString *core = [flag substringWithRange:NSMakeRange(6, 37)];
            if ([core substringWithRange:NSMakeRange(0, 2)] != [core substringWithRange:NSMakeRange(18, 2)]) {
                [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
            }
            if ([core substringWithRange:NSMakeRange(3, 1)] != [core substringWithRange:NSMakeRange(5, 1)]) {
                [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
            }
        } else {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        
        if(![@"pVz" isEqualToString:[self r:[flag substringWithRange:NSMakeRange([self y], [self z])]]]) {
             [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        if ( ((int) [flag characterAtIndex:10] != 95) ||
            ((int) [flag characterAtIndex:14] != 0x5f) ||
            ((int) [flag characterAtIndex:19] != 95)
           ) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
         
        if(([flag characterAtIndex:33] - 48) != [self z]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        if (
            !(((int) [flag characterAtIndex:10] == (int) [flag characterAtIndex:22]) &&
            ((int) [flag characterAtIndex:14] == (int) [flag characterAtIndex:34]) &&
            ((int) [flag characterAtIndex:19] == (int) [flag characterAtIndex:38])
            ))
        {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
            
        if(![@"OhyK" isEqualToString:[self r:[flag substringWithRange:NSMakeRange(15, 4)]]]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
         
        if(![@"fOO" isEqualToString:[self w:[flag substringWithRange:NSMakeRange(11, 3)]]] ||
           ![@"fO" isEqualToString:[[self w:[flag substringWithRange:NSMakeRange(11, 3)]] substringWithRange:NSMakeRange(0,2)]]) {
            [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
        }
        
        NSString *frag = [flag substringWithRange:NSMakeRange(23, 10)];
        for (int i=0; i < 10; i+=2) {
            if ([self p:[frag substringWithRange:NSMakeRange(i, 2)] withIndex:i % 5]) {
                [NSException raise:@"Invalid flag" format:@"flag %@ is invalid", flag];
            }
        }
        
    } @catch(NSException *err) {
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)p:(NSString *)ww withIndex:(NSInteger)i{
    return ![[[self m:ww] lowercaseString] isEqualToString:hashes[i]];
}

- (IBAction)invoke:(id)sender {
    NSString *flag = txtFlag.text;
    NSString *res = @"";
    if ([self checkFlag:flag]) {
        res = @"Flag is correct!";
    } else {
        res = @"Invalid flag";
    }
    
    UIAlertController * alert = [UIAlertController
                    alertControllerWithTitle:@"Check Flag"
                                     message:res
                              preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action){}];
    [alert addAction:okAction];
    [self presentViewController:alert animated:YES completion:nil];
}

- (NSString *)r:(NSString *)s {
    NSString *res = @"";
    for (int i=0; i< s.length; i++) {
        unichar c = [s characterAtIndex:i];
        if (c >= 'a' && c <= 's') {
            c = (unichar) (c + 7);
        } else if (c >= 'A' && c <= 'S') {
            c = (unichar) (c + 7);
        } else if (c >= 't' && c <= 'z') {
            c = (unichar) (c - 19);
        } else if (c >= 'T' && c <= 'Z') {
            c = (unichar) (c - 19);
        }
        res = [res stringByAppendingFormat:@"%C", c];
    }
    return res;
}

- (NSString *)x:(NSString *)s {
    NSString *res = @"";
    for (int i=0; i< s.length; i++) {
        unichar c = [s characterAtIndex:i];
        if (c >= 'a' && c <= 's') {
            c = (unichar) (c + 7);
        } else if (c >= 'A' && c <= 'S') {
            c = (unichar) (c + 7);
        } else if (c >= 't' && c <= 'z') {
            c = (unichar) (c - 19);
        } else if (c >= 'T' && c <= 'Z') {
            c = (unichar) (c - 19);
        }
        res = [res stringByAppendingFormat:@"%C", c];
    }
    return res;
}

- (int) y {
    return (7 << 3 >> 1 << 4 >> 1 >> 2 >> 3) * 5;
}

- (int) z {
    return sqrt((pow(2.0,5.0) + pow(2.0, 2.0)) / 4) ;
}

- (NSString *) w:(NSString *)s {
    NSString *res = @"";
    unsigned long len = [s length];
    char buffer[len];
    strncpy(buffer, [s UTF8String], len);
    for(int i = 0; i < sizeof(buffer); i++) {
       char current = buffer[i] ^ box[(int) buffer[i] % 25];
       res = [res stringByAppendingFormat:@"%C", current];
    }
    
    return res;
}

- (NSString *)m:(NSString *)s{
    const char *cStr = [s UTF8String];
        unsigned char result[CC_MD5_DIGEST_LENGTH];
        CC_MD5( cStr, (CC_LONG)strlen(cStr), result );

        return [NSString stringWithFormat:
            @"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            result[0], result[1], result[2], result[3],
            result[4], result[5], result[6], result[7],
            result[8], result[9], result[10], result[11],
            result[12], result[13], result[14], result[15]
        ];
}


//antifrida tricks got from
//github.com/rustymagnet3000/ios_debugger_challenge/blob/master/debugger_challenge/objc_source/frida_detection.m

/* check each thread name against Frida strings */
+(BOOL)f{
    NSInteger countNameThreads = 0;
    thread_act_array_t thread_list;
    mach_msg_type_number_t thread_count = 0;
    const task_t    this_task = mach_task_self();
    const thread_t  this_thread = mach_thread_self();
    
    kern_return_t kr = task_threads (this_task, &thread_list, &thread_count);
    if (kr != KERN_SUCCESS){
        NSLog(@"error getting task_threads: %s\n", mach_error_string(kr));
        return NO;
    }
    
    char thread_name[THREAD_NAME_MAX];
    NSMutableArray *namedThreads = [NSMutableArray new];
    
    for (int i = 0; i < thread_count; i++){
        pthread_t pt = pthread_from_mach_thread_np (thread_list[i]);
        if (pt) {
            thread_name[0] = '\0';
            int rc = pthread_getname_np (pt, thread_name, THREAD_NAME_MAX);
            uint64_t tid;
            pthread_threadid_np (pt, &tid);
            if (thread_name[0] != '\0' && rc == 0)
                [namedThreads addObject: [NSString stringWithCString: thread_name encoding:NSASCIIStringEncoding]];
        }
    }
    
    
    countNameThreads = [ViewController x:namedThreads];
    
    mach_port_deallocate(this_task, this_thread);
    vm_deallocate(this_task, (vm_address_t)thread_list, sizeof(thread_t) * thread_count);
    
    return countNameThreads > 0 ? YES : NO;
}

// Check Frida Default Port
+(BOOL)g{
    int result, sock;
    struct sockaddr_in sa = {0};

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(HOSTNAME);

    for( int i = START;  i < END; i++ ){
        sa.sin_port = htons( i );
        sock = socket( AF_INET, SOCK_STREAM, 0 );
        result = connect( sock , ( struct sockaddr *) &sa , sizeof sa );
        if ( result == 0 && ( i = FRIDA_DEFAULT)) {
            return YES;
        }
        close ( sock );
    }

    return NO;
}

+(NSInteger) x: (NSMutableArray *)strItems {
    
    NSInteger count = 0;
    for (NSString *item in strItems) {
        for (int i=0 ; i<MAX_FRIDA_STRINGS; i++) {
            NSString *friStr= [NSString stringWithUTF8String:frida_strings[i]];
            if ([item containsString:friStr])
                count++;
        }
    }
    return count;
}

@end
