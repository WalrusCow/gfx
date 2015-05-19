#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <list>
#include <string>
#include <unordered_map>

#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow();

 protected:
  void keyPressEvent(QKeyEvent *event);

 private:
  void createActions();
  void createMenu();

  QAction* newAction(const std::string& title, const std::string& tip, int key);


  QMenu* appMenu;
  std::list<QAction*> appMenuActions;
  QMenu* drawMenu;
  std::list<QAction*> drawMenuActions;

  std::unordered_map<int, QAction*> shortcutActions;

  Viewer* m_viewer;
};
