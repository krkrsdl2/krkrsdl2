/**
 * @file     menu.cc
 * @author   Hikaru Terazono <3c1u@vulpesgames.tokyo>
 * @brief    Cross-platform menu.dll implementation.
 * @version  0.1
 * @date     2020-08-09
 *
 * @copyright Copyright (c) 2020 Hikaru Terazono (3c1u). All rights reserved.
 *
 */

#include "menu.h"
#include "ncbind/ncbind.hpp"

#include <codecvt>
#include <locale>

extern bool TVPEncodeUTF8ToUTF16(tjs_string &output, const std::string &source);
// extern bool TVPEncodeUTF16ToUTF8(std::string &output, const tjs_string
// &source);

class MenuItemBase : public NativeMenuItem {
public:
  MenuItemBase() : NativeMenuItem() {}
  virtual ~MenuItemBase() {}

  void add(MenuItemBase *item) {
    assert("menu item should not be null" && item != nullptr);
    NativeMenuItem::addItem(item);
  }

  void insert(MenuItemBase *item, int at) {
    assert("menu item should not be null" && item != nullptr);
    NativeMenuItem::insert(item, at);
  }

  void remove(MenuItemBase *item) {
    assert("menu item should not be null" && item != nullptr);
    NativeMenuItem::remove(item);
  }

  tTJSVariant getCaption() {
    tjs_string output;
    auto       caption = NativeMenuItem::getCaption();
    TVPEncodeUTF8ToUTF16(output, caption);
    return ttstr(output);
  }

  void setCaption(tTJSVariant caption) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;

    auto str = caption.AsStringNoAddRef();

    if (str) {
      auto caption_ = converter.to_bytes(
          tjs_string(str->LongString ? str->LongString : str->ShortString,
                     str->GetLength()));
      NativeMenuItem::setCaption(caption_);
    }
  }
};

void PostRegistMenu() {
  tTJSVariant var;
  TVPExecuteScript(TJS_W(R"__TJS_SCRIPT___(
/**
 * @file     cocoaMenuGlue.tjs
 * @author   Hikaru Terazono <3c1u@vulpesgames.tokyo>
 * @brief    Cross-platform menu.dll implementation.
 * @version  0.1
 * @date     2020-08-09
 *
 * @copyright Copyright (c) 2020 Hikaru Terazono (3c1u). All rights reserved.
 *
 */

class MenuListener {
	function updateMenuItem(item: MenuItem) {}
	function updateMenuList(parent: MenuItem ) {}
	function selectMenuItem(item: MenuItem) {}
};

class MenuItem extends MenuItemBase {
  var m_parent: Array;
  var m_children: MenuItem;
  var m_window: Window;
  var m_listener: MenuListener;

  function MenuItem(window: Window, caption: string="", listener: MenuListener=null) {
    super.MenuItemBase();

    m_children = new Array();
    m_window = window;
    m_listener = listener;

    this.caption = caption;
  }
  
  function finalize {
		super.finalize(...);
	}

  function updateItem() {
		if( m_listener != null ) {
			m_listener.updateMenuItem( this );
		}
	}

	function updateList() {
		if( m_listener != null ) {
			m_listener.updateMenuList( this );
		}
	}

	function selectItem() {
		if( m_listener != null ) {
			listener.selectMenuItem( this );
		}
	}

  function add(item: MenuItem) {
		m_children.add(item);

    item.m_parent = this;
    super.add(item);

		updateList();
	}

  function insert(index, item) {
    // swap arguments
		if (typeof index === "Object") {
			if (typeof item !== "Object")
        insert(item, index);
			return;
		}

		m_children.remove(item);
		m_children.insert(index, item);

    item.m_parent = this;

    super.insert(item, index);

		updateList();
	}

	function remove(item: MenuItem) {
		m_children.remove(item);
		item.m_parent = null;
		updateList();
	}

  property parent { getter{ return m_parent; } }

	property children { getter{ return m_children; } }

  property index {
		setter(c) {
			if (m_parent) {
        m_parent.insert(c, this); 
      }
		}
		getter {
			return super.index;
		}
	}

  property root {
		getter {
			var r = this;
			while( r.m_parent != null ) {
				r = r.m_parent;
			}
			return r;
		}
	}

  function onClick() {
		selectItem();
	}

	function dump() {
		var mes: string = "";
		var par: MenuItem = parent_;
		while( par != null ) {
			mes += "    ";
			par = par.parent_;
		}
		if( caption_ == "-" ) {
			mes += "-------------------------------";
		} else {
			mes += caption_;
		}
		Debug.message( mes );
		var count:int = children_.count;
		for( var i:int = 0; i < count; i++ ) {
			children_[i].dump();
		}
	}
}

class _WindowMenuPropHook {
    property menu {
        getter {
            if (typeof this.__menu === "undefined") {
                this.__menu = new MenuItem(this);
                
                if (typeof this.__menu.activate !== "undefined") {
                    // TODO: the activation should follow the status of the window
                    this.__menu.activate();
                }
            }

            return this.__menu;
        }
    }
}

var _hook = new _WindowMenuPropHook();
&global.Window.menu = &_hook.menu;
      )__TJS_SCRIPT___"),
                   &var);
}

NCB_REGISTER_CLASS(MenuItemBase) {
  Constructor();

  NCB_METHOD(add);
  NCB_METHOD(insert);
  NCB_METHOD(remove);

  NCB_METHOD(activate);
  NCB_METHOD(popup);

  NCB_PROPERTY(caption, getCaption, setCaption);
  // NCB_PROPERTY(shortcut, getKeyModifier, setKeyModifier);

  NCB_PROPERTY(enabled, getEnabled, setEnabled);
  NCB_PROPERTY(visible, getVisible, setVisible);
  NCB_PROPERTY(index, getIndex, setIndex);
  NCB_PROPERTY(radio, getRadio, setRadio);
  NCB_PROPERTY(group, getGroup, setGroup);

  // NCB_PROPERTY_RO(root, getRoot);
  // NCB_PROPERTY_RO(parent, getParent);
  // NCB_PROPERTY_RO(children, getChildren);
};

NCB_POST_REGIST_CALLBACK(PostRegistMenu);
