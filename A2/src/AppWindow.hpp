#pragma once

#include <functional>
#include <list>
#include <string>
#include <unordered_map>

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QAction>
#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow();
  void updateMessage(const std::string& mode, double near, double far);

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
  std::list<QAction*> quitMenuActions;

  QMenu* modeMenu;
  std::list<QAction*> modeMenuActions;

  std::unordered_map<int, QAction*> shortcutActions;
  Viewer* viewer;
  QLabel* messageLabel;
};
