#pragma once
#import <UIKit/UIKit.h>
#include "../ModLoader/ModLoader.hpp"

// ═══════════════════════════════════════════════════════════════
//  AUMF Mod Menu  —  UIKit overlay, injected over the game window
//  Activated by a 4-finger tap (configurable)
// ═══════════════════════════════════════════════════════════════

@interface AUMFMenuCell : UITableViewCell
@property (nonatomic, strong) UISwitch* toggle;
@end
@implementation AUMFMenuCell
- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString*)id {
    self = [super initWithStyle:style reuseIdentifier:id];
    _toggle = [[UISwitch alloc] init];
    [self.contentView addSubview:_toggle];
    self.backgroundColor = [UIColor colorWithWhite:0.12 alpha:1.0];
    self.textLabel.textColor = UIColor.whiteColor;
    return self;
}
- (void)layoutSubviews {
    [super layoutSubviews];
    _toggle.frame = CGRectMake(self.bounds.size.width - 60, 8, 51, 31);
}
@end

// ─────────────────────────────────────────────

@interface AUMFMenuController : UIViewController <UITableViewDataSource, UITableViewDelegate>
@property (nonatomic, strong) UITableView* tableView;
@property (nonatomic, strong) UILabel*     titleLabel;
@property (nonatomic, strong) UILabel*     versionLabel;
@end

@implementation AUMFMenuController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor colorWithWhite:0.08 alpha:0.95];
    self.view.layer.cornerRadius = 16;
    self.view.layer.borderColor  = [UIColor colorWithRed:0.8 green:0.1 blue:0.1 alpha:1].CGColor;
    self.view.layer.borderWidth  = 2;
    self.view.clipsToBounds = YES;

    // Title bar
    UIView* titleBar = [[UIView alloc] initWithFrame:CGRectMake(0,0,self.view.bounds.size.width,48)];
    titleBar.backgroundColor = [UIColor colorWithRed:0.7 green:0.05 blue:0.05 alpha:1];

    _titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(12, 0, 200, 48)];
    _titleLabel.text = @"☠ AUMF Mod Menu";
    _titleLabel.textColor = UIColor.whiteColor;
    _titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [titleBar addSubview:_titleLabel];

    // Close button
    UIButton* closeBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    closeBtn.frame = CGRectMake(self.view.bounds.size.width - 48, 4, 40, 40);
    [closeBtn setTitle:@"✕" forState:UIControlStateNormal];
    [closeBtn setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
    closeBtn.titleLabel.font = [UIFont boldSystemFontOfSize:18];
    [closeBtn addTarget:self action:@selector(dismiss) forControlEvents:UIControlEventTouchUpInside];
    [titleBar addSubview:closeBtn];

    [self.view addSubview:titleBar];

    // Version label
    _versionLabel = [[UILabel alloc] initWithFrame:CGRectMake(0,48,self.view.bounds.size.width,24)];
    _versionLabel.text = @"AUMF v1.0.0  •  Among Us iOS";
    _versionLabel.textColor = [UIColor colorWithWhite:0.5 alpha:1];
    _versionLabel.font = [UIFont systemFontOfSize:11];
    _versionLabel.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:_versionLabel];

    // Table
    _tableView = [[UITableView alloc] initWithFrame:
        CGRectMake(0, 72, self.view.bounds.size.width, self.view.bounds.size.height - 72)
        style:UITableViewStylePlain];
    _tableView.backgroundColor = UIColor.clearColor;
    _tableView.dataSource = self;
    _tableView.delegate   = self;
    _tableView.separatorColor = [UIColor colorWithWhite:0.2 alpha:1];
    [_tableView registerClass:[AUMFMenuCell class] forCellReuseIdentifier:@"mod"];
    [self.view addSubview:_tableView];
}

- (void)dismiss {
    [UIView animateWithDuration:0.25 animations:^{
        self.view.alpha = 0;
        self.view.transform = CGAffineTransformMakeScale(0.9, 0.9);
    } completion:^(BOOL _) {
        [self.view removeFromSuperview];
        [self removeFromParentViewController];
    }];
}

- (NSInteger)tableView:(UITableView*)tv numberOfRowsInSection:(NSInteger)s {
    return (NSInteger)ModLoader::g_mods.size();
}

- (UITableViewCell*)tableView:(UITableView*)tv cellForRowAtIndexPath:(NSIndexPath*)ip {
    AUMFMenuCell* cell = [tv dequeueReusableCellWithIdentifier:@"mod" forIndexPath:ip];
    auto& m = ModLoader::g_mods[(size_t)ip.row];
    ModInfo info = m.instance->GetInfo();
    cell.textLabel.text = [NSString stringWithFormat:@"%s  v%s",
                           info.name.c_str(), info.version.c_str()];
    cell.detailTextLabel.text = [NSString stringWithUTF8String:info.description.c_str()];
    cell.toggle.on = m.instance->enabled;
    cell.toggle.tag = ip.row;
    [cell.toggle addTarget:self action:@selector(toggleMod:)
         forControlEvents:UIControlEventValueChanged];
    return cell;
}
- (CGFloat)tableView:(UITableView*)tv heightForRowAtIndexPath:(NSIndexPath*)ip { return 52; }

- (void)toggleMod:(UISwitch*)sw {
    if ((size_t)sw.tag < ModLoader::g_mods.size())
        ModLoader::g_mods[(size_t)sw.tag].instance->enabled = sw.on;
}

@end

// ─────────────────────────────────────────────
//  MenuOverlay singleton — shows/hides the menu
// ─────────────────────────────────────────────
@interface AUMFMenuOverlay : NSObject
+ (void)install;
+ (void)toggle;
@end

static AUMFMenuController* g_menuVC = nil;

@implementation AUMFMenuOverlay

+ (void)install {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIWindow* win = [[[UIApplication sharedApplication] windows] firstObject];
        UIGestureRecognizer* tap =
            [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
        ((UITapGestureRecognizer*)tap).numberOfTouchesRequired = 4;
        [win addGestureRecognizer:tap];
        NSLog(@"[AUMF] Mod menu installed — 4-finger tap to open");
    });
}

+ (void)handleTap:(UIGestureRecognizer*)gr {
    if (gr.state == UIGestureRecognizerStateEnded)
        [self toggle];
}

+ (void)toggle {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIWindow* win = [[[UIApplication sharedApplication] windows] firstObject];
        if (g_menuVC && g_menuVC.view.superview) {
            [g_menuVC dismiss]; return;
        }
        CGRect screen = UIScreen.mainScreen.bounds;
        CGFloat w = MIN(screen.size.width * 0.85f, 360);
        CGFloat h = MIN(screen.size.height * 0.7f, 520);
        g_menuVC = [[AUMFMenuController alloc] init];
        g_menuVC.view.frame = CGRectMake((screen.size.width-w)/2,
                                          (screen.size.height-h)/2, w, h);
        g_menuVC.view.alpha = 0;
        g_menuVC.view.transform = CGAffineTransformMakeScale(0.85, 0.85);
        [win addSubview:g_menuVC.view];
        [UIView animateWithDuration:0.3
                              delay:0
             usingSpringWithDamping:0.7
              initialSpringVelocity:0.5
                            options:0
                         animations:^{
            g_menuVC.view.alpha = 1;
            g_menuVC.view.transform = CGAffineTransformIdentity;
        } completion:nil];
    });
}

@end
