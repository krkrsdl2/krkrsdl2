/**
 * @file     cocoa.mm
 * @author   Hikaru Terazono <3c1u@vulpesgames.tokyo>
 * @brief    menu.dll implementation for macOS.
 * @version  0.1
 * @date     2020-08-09
 *
 * @copyright Copyright (c) 2020 Hikaru Terazono (3c1u). All rights reserved.
 *
 */

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include <locale>
#include <string>
#include <string_view>
#include <vector>

#include <assert.h>

#include "common.h"

struct ShortcutKey {
  NSUInteger  keyMask;
  std::string keyEquivalent;
};

static void parseShortcutKey(std::string const &keyComb, ShortcutKey &out) {
  const std::string delimeter{"+"};

  std::string            tstr = keyComb + delimeter;
  long                   l = tstr.length(), sl = delimeter.length();
  std::string::size_type pos = 0, prev = 0;

  bool shift = false;

  for (; pos < l && (pos = tstr.find(delimeter, pos)) != std::string::npos;
       prev = (pos += sl)) {
    std::string_view sub = tstr.substr(prev, pos - prev);

    if (sub == "Alt") {
      out.keyMask |= NSEventModifierFlagOption;
    } else if (sub == "Ctrl") {
      out.keyMask |= NSEventModifierFlagCommand;
      // NSEventModifierFlagControl
    } else if (sub == "Shift") {
      shift = true;
    } else if (sub == "Enter") {
      out.keyEquivalent += '\r';
    } else if (sub.size() == 1) {
      if (shift)
        out.keyEquivalent += toupper(sub[0]);
      else
        out.keyEquivalent += tolower(sub[0]);
    } else if (sub.size() > 1 && sub[0] == 'F') {
      auto const fnum_str = std::string(sub).substr(1);
      int        fnum     = 0;

      try {
        fnum = std::stoi(fnum_str);
      } catch (std::invalid_argument arg) {
        // std::cerr << "[krkrcocoa] invalid function key: " << fnum_str
        //           << std::endl;
        continue;
      }

      // force-encode NSF*FunctionKey into UTF-8
      out.keyEquivalent += char(0xEF);
      out.keyEquivalent += char(0x9C);
      out.keyEquivalent += char(0x80 | (3 + fnum));
    } else {
      // unsupported key
      // std::cerr << "[krkrcocoa] unsupported key: " << sub << std::endl;
    }
  }
}

@interface KrkrMenuItem : NSMenuItem {
  _NativeMenuItem_Impl *m_menuItem;
}
- (id)initWithMenuItemImpl:(_NativeMenuItem_Impl *)menuItem;
- (IBAction)handleClick:(id)sender;

@end

class _NativeMenuItem_Impl {
public:
  _NativeMenuItem_Impl(NativeMenuItem *interface, std::string caption = "")
      : m_refCount(1), m_interface(interface), m_menu(nullptr), m_item(nullptr),
        m_parent(nullptr), m_children(), m_caption(caption), m_key(""),
        m_checked(false), m_enabled(true), m_visible(true), m_radio(false),
        m_activated(false), m_radioGroup(true), m_delegate(nullptr) {
    // create an empty menu stub
  }

  virtual ~_NativeMenuItem_Impl() {
    for (auto &v : m_children) {
      v->m_parent = nullptr;
      v->release();
    }

    if (m_menu) {
      [m_menu release];
    }

    if (m_item) {
      [m_item release];
    }
  }

  void activate() {
    ensureMenu();
    m_activated = true;
    [NSApp setMainMenu:m_menu];
  }

  void addItem(_NativeMenuItem_Impl *item) {
    item->retain();

    if (item->m_parent)
      item->m_parent->remove(item);

    item->ensureMenuItem();
    this->ensureMenu();

    item->m_parent = this;
    m_children.push_back(item);

    [m_menu addItem:item->m_item];
  }

  void insert(_NativeMenuItem_Impl *item, int at) {
    item->retain();

    if (item->m_parent)
      item->m_parent->remove(item);

    item->ensureMenuItem();
    this->ensureMenu();

    item->m_parent = this;
    m_children.insert(m_children.begin() + at, item);

    [m_menu insertItem:item->m_item atIndex:static_cast<NSInteger>(at)];
  }

  void retain() { m_refCount++; }

  bool release(bool respectIntf = true) {
    if (respectIntf) {
      return m_interface->release();
    }

    if (--m_refCount == 0) {
      assert("there should be no parent" && !m_parent);
      delete this;
      return true;
    }

    return false;
  }

  void popup(int flags, int x, int y) {
    if (!m_menu)
      return;

    [m_menu popUpMenuPositioningItem:m_item
                          atLocation:NSMakePoint(x, y)
                              inView:nil];
  }

  void remove(_NativeMenuItem_Impl *item) {
    if (!item || item->m_parent != this) {
      return;
    }

    auto p = std::find(m_children.begin(), m_children.end(), item);

    if (p != m_children.end()) {
      m_children.erase(p);

      item->m_parent = nullptr;
      item->release();

      [m_menu removeItem:item->m_item];
    }
  }

  void *getMenuRef() const { return reinterpret_cast<void *>(m_menu); }

  void *getMenuItemRef() const { return reinterpret_cast<void *>(m_item); }

  void setCaption(std::string const &caption) {
    // separator
    if (m_caption != "-" && caption == "-") {
      auto at   = getIndex();
      m_item    = [NSMenuItem separatorItem];
      m_caption = caption;
      if (m_parent) {
        [(m_parent->m_menu) removeItem:m_item];
        [(m_parent->m_menu) insertItem:m_item
                               atIndex:static_cast<NSInteger>(at)];
      }
      return;
    } else if (m_caption == "-" && caption != "-") {
      auto at   = getIndex();
      m_item    = nil;
      m_caption = caption;
      ensureMenuItem();
      if (m_parent) {
        [(m_parent->m_menu) removeItem:m_item];
        [(m_parent->m_menu) insertItem:m_item
                               atIndex:static_cast<NSInteger>(at)];
      }
      return;
    }

    m_caption = caption;

    auto sanitizedCaption = m_caption;

    sanitizedCaption.erase(
        std::remove(sanitizedCaption.begin(), sanitizedCaption.end(), '&'),
        sanitizedCaption.end());

    auto theCaption = [NSString stringWithUTF8String:sanitizedCaption.c_str()];

    if (m_menu) {
      [m_menu setTitle:theCaption];
    }
    if (m_item) {
      [m_item setTitle:theCaption];
    }
  }

  std::string const &getCaption() const { return m_caption; }

  void setChecked(bool flag) {
    m_checked = flag;

    ensureMenuItem();

    [m_item
        setState:m_checked ? NSControlStateValueOn : NSControlStateValueOff];

    // if the item is a radio button, uncheck other elements in the group.
    if (m_radio && m_parent && flag) {
      for (auto &i : m_parent->m_children) {
        if (i == this || i->getRadio() ||
            i->m_radioGroup != this->m_radioGroup) {
          continue;
        }

        i->setChecked(false);
      }
    }
  }

  bool getChecked() const { return m_checked; }

  void setKeyEquivalent(std::string const &key) {
    m_key = key;

    // treat special key combinations (e.g. Alt+F4 -> Cmd+Q)
    if (key == "Alt+F4") {
      [m_item setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
      [m_item setKeyEquivalent:@"q"];
      return;
    }

    ShortcutKey parsed{0};
    parseShortcutKey(key, parsed);

    [m_item setKeyEquivalentModifierMask:parsed.keyMask];
    [m_item setKeyEquivalent:[NSString stringWithUTF8String:parsed.keyEquivalent
                                                                .c_str()]];
  }

  std::string const &getKeyEquivalent() const { return m_key; }

  _NativeMenuItem_Impl const *getRoot() const {
    if (m_parent)
      return m_parent->getRoot();

    return this;
  }

  _NativeMenuItem_Impl *getRoot() {
    if (m_parent)
      return m_parent->getRoot();

    return this;
  }

  _NativeMenuItem_Impl const *getParent() const { return this->m_parent; }

  _NativeMenuItem_Impl *getParent() { return this->m_parent; }

  std::vector<_NativeMenuItem_Impl *> const &getChildren() const {
    return this->m_children;
  }

  void setEnabled(bool flag) {
    m_enabled = flag;

    if (!m_item)
      return;

    [m_item setEnabled:m_enabled];
  }

  bool getEnabled() const { return m_enabled; }

  void setVisible(bool flag) {
    m_visible = flag;

    if (!m_item)
      return;

    [m_item setHidden:!m_visible];
  }

  bool getVisible() const { return m_visible; }

  int getIndex() const {
    if (!m_parent)
      return -1;

    auto const &siblings = m_parent->m_children;
    auto        p        = std::find(siblings.begin(), siblings.end(), this);

    if (p == siblings.end()) {
      return -1;
    }

    return std::distance(siblings.begin(), p);
  }

  void setIndex(int index) {
    if (m_parent)
      m_parent->insert(this, index);
  }

  void setRadio(bool flag) { m_radio = flag; }

  bool getRadio() const { return m_radio; }

  void setGroup(int group) { m_radioGroup = group; }

  int getGroup() const { return m_radioGroup; }

  void handleClick() {
    if (m_delegate)
      m_delegate->handleClick(this->m_interface);
  }

  NativeMenuItem *getIntf() const { return m_interface; }

  void setDelegate(INativeMenuItemDelegate *delegate) { m_delegate = delegate; }

private:
  size_t                              m_refCount;
  NativeMenuItem *                    m_interface;
  NSMenu *                            m_menu;
  NSMenuItem *                        m_item;
  _NativeMenuItem_Impl *              m_parent;
  std::vector<_NativeMenuItem_Impl *> m_children;
  std::string                         m_caption;
  std::string                         m_key;
  bool                                m_checked;
  bool                                m_enabled;
  bool                                m_visible;
  bool                                m_radio;
  bool                                m_activated;
  int                                 m_radioGroup;
  INativeMenuItemDelegate *           m_delegate;

  void ensureMenuItem() {
    if (!m_item) {
      m_item = [[KrkrMenuItem alloc] initWithMenuItemImpl:this];
    }

    setCaption(m_caption);

    if (m_menu) {
      [m_item setSubmenu:m_menu];
    }
  }

  void ensureMenu() {
    if (!m_menu) {
      m_menu = [[NSMenu alloc] init];
    }

    setCaption(m_caption);

    if (m_item) {
      [m_item setSubmenu:m_menu];
    }
  }
};

/* クラスの実装部 */
@implementation KrkrMenuItem : NSMenuItem
- (id)initWithMenuItemImpl:(_NativeMenuItem_Impl *)menuItem {
  if (self = [super init]) {
    m_menuItem = menuItem;
    [self setTarget:self];
    [self setAction:@selector(handleClick:)];
  }

  return self;
}

- (IBAction)handleClick:(id)sender {
  m_menuItem->handleClick();
}

@end

NativeMenuItem::NativeMenuItem(std::string caption, void *_nativeWindow) {
  m_impl = new _NativeMenuItem_Impl(this, caption);
  m_impl->retain();
}

NativeMenuItem::~NativeMenuItem() {
  if (m_impl)
    m_impl->release();
}

void NativeMenuItem::activate() { m_impl->activate(); }

void NativeMenuItem::addItem(NativeMenuItem *item) {
  m_impl->addItem(item->m_impl);
}

void NativeMenuItem::insert(NativeMenuItem *item, int at) {
  m_impl->insert(item->m_impl, at);
}

void NativeMenuItem::retain() { m_impl->retain(); }

bool NativeMenuItem::release() {
  if (!m_impl) {
    return true;
  }

  bool res = m_impl->release(false);
  if (m_impl && res) {
    m_impl = nullptr;
    delete this;
  }
  return res;
}

void NativeMenuItem::popup(int flags, int x, int y) {
  m_impl->popup(flags, x, y);
}

void NativeMenuItem::remove(NativeMenuItem *item) {
  m_impl->remove(item->m_impl);
}

void NativeMenuItem::setCaption(std::string const &caption) {
  m_impl->setCaption(caption);
}

std::string const &NativeMenuItem::getCaption() { return m_impl->getCaption(); }

void NativeMenuItem::setChecked(bool flag) { m_impl->setChecked(flag); }

bool NativeMenuItem::getChecked() const { return m_impl->getChecked(); }

void NativeMenuItem::setKeyEquivalent(std::string const &key) {
  return m_impl->setKeyEquivalent(key);
}

std::string const &NativeMenuItem::getKeyEquivalent() {
  return m_impl->getKeyEquivalent();
}

void NativeMenuItem::setEnabled(bool flag) { m_impl->setEnabled(flag); }

bool NativeMenuItem::getEnabled() const { return m_impl->getEnabled(); }

void NativeMenuItem::setVisible(bool flag) { m_impl->setVisible(flag); }

bool NativeMenuItem::getVisible() const { return m_impl->getVisible(); }

void NativeMenuItem::setRadio(bool flag) { m_impl->setRadio(flag); }

bool NativeMenuItem::getRadio() const { return m_impl->getRadio(); }

void NativeMenuItem::setIndex(int index) { m_impl->setIndex(index); }

int NativeMenuItem::getIndex() const { return m_impl->getIndex(); }

void NativeMenuItem::setGroup(int group) { m_impl->setGroup(group); }

int NativeMenuItem::getGroup() const { return m_impl->getGroup(); }

NativeMenuItem *NativeMenuItem::getRoot() {
  return m_impl->getRoot()->getIntf();
}

NativeMenuItem *NativeMenuItem::getParent() {
  return m_impl->getParent()->getIntf();
}

std::vector<NativeMenuItem *> NativeMenuItem::getChildren() {
  std::vector<NativeMenuItem *> vec;
  auto const &                  children = m_impl->getChildren();
  for (auto &c : children) {
    vec.push_back(c->getIntf());
  }
  return vec;
}

void NativeMenuItem::setDelegate(INativeMenuItemDelegate *delegate) {
  m_impl->setDelegate(delegate);
}

#endif // __APPLE__
