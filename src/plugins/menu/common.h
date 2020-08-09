/**
 * @file     common.h
 * @author   Hikaru Terazono <3c1u@vulpesgames.tokyo>
 * @brief    Cross-platform menu.dll implementation.
 * @version  0.1
 * @date     2020-08-09
 *
 * @copyright Copyright (c) 2020 Hikaru Terazono (3c1u). All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>

class _NativeMenuItem_Impl;
class NativeMenuItem;

class INativeMenuItemDelegate {
public:
  virtual void handleClick(NativeMenuItem *sender);
  virtual ~INativeMenuItemDelegate();
};

class NativeMenuItem {
public:
  NativeMenuItem(std::string caption = "", void *nativeWindow = nullptr);
  virtual ~NativeMenuItem();

  void activate();
  void addItem(NativeMenuItem *item);
  void insert(NativeMenuItem *item, int at);
  void retain();
  bool release();
  void popup(int flags, int x, int y);
  void remove(NativeMenuItem *item);

  void               setCaption(std::string const &caption);
  std::string const &getCaption();

  void setChecked(bool flag);
  bool getChecked() const;

  void               setKeyModifier(std::string const &modifier);
  std::string const &getKeyModifier();

  void setEnabled(bool flag);
  bool getEnabled() const;

  void setVisible(bool flag);
  bool getVisible() const;

  void setIndex(int index);
  int  getIndex() const;

  void setRadio(bool flag);
  bool getRadio() const;

  void setGroup(int group);
  int  getGroup() const;

  NativeMenuItem *              getRoot();
  NativeMenuItem *              getParent();
  std::vector<NativeMenuItem *> getChildren();

  void setDelegate(INativeMenuItemDelegate *delegate);

private:
  _NativeMenuItem_Impl *m_impl;
};
