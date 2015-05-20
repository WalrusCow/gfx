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

 public slots:
  void newGame();
  void resetView();
  void faceMode();
  void wireMode();
  void multiMode();
  void slowSpeed();
  void medSpeed();
  void fastSpeed();
  void gameTick();

 public:
  AppWindow();

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
      int shortcut);

  QTimer* gameTicker;

  static const int SLOW_MS;
  static const int MEDIUM_MS;
  static const int FAST_MS;

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
