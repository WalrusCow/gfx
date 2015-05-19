#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

#include "game.hpp"
#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow();
  void newGame();

 protected:
  void keyPressEvent(QKeyEvent *event);

 private:
  void createActions();
  void createMenu();

  QAction* newMenuAction(
      const std::string& title,
      QActionGroup* actionGroup,
      const std::string& tip,
      std::list<QAction*>& menuList,
      int shortcut,
      const std::function<void()>& onTrigger);

  QAction* newMenuAction(
      const std::string& title,
      const std::string& tip,
      std::list<QAction*>& menuList,
      int shortcut,
      const std::function<void()>& onTrigger);

  QTimer* gameTicker;

  QMenu* appMenu;
  std::list<QAction*> appMenuActions;
  QMenu* drawMenu;
  std::list<QAction*> drawMenuActions;
  QMenu* speedMenu;
  std::list<QAction*> speedMenuActions;

  std::unordered_map<int, QAction*> shortcutActions;

  Viewer* m_viewer;
  Game game;
};
