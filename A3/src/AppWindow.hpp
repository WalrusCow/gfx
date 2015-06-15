// William McDonald 20418145 wmcdonal
#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "scene.hpp"
#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow(std::unique_ptr<SceneNode> rootNode);

 protected:
  void keyPressEvent(QKeyEvent* event) override;

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

  QMenu* editMenu;
  std::list<QAction*> editMenuActions;

  QMenu* optionsMenu;
  std::list<QAction*> optionsMenuActions;

  std::unordered_map<int, QAction*> shortcutActions;

  Viewer* viewer;
};
