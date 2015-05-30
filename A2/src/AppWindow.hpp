#pragma once

#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow();

 protected:
  void keyPressEvent(QKeyEvent* event);

 private:
  void createActions();
  void createMenu();

  QAction* newMenuAction(
      const std::string& title,
      QActionGroup* actionGroup,
      const std::string& tip,
      std::list<QAction*>& menuList,
      int shortcut,
      const std::function<void()> onTrigger);

  // Each menu itself
  QMenu* appMenu;

  QMenu* modeMenu;
  std::list<QAction*> modeMenuActions;

  std::unordered_map<int, QAction*> shortcutActions;

  std::vector<QAction*> m_menu_actions;
  Viewer* m_viewer;
};
