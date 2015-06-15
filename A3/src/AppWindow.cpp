// William McDonald 20418145 wmcdonal
#include "AppWindow.hpp"

#include <iostream>

#include <QtWidgets>

AppWindow::AppWindow(std::unique_ptr<SceneNode> rootNode) {
  setWindowTitle("Puppet Master");

  QGLFormat glFormat;
  glFormat.setVersion(3,3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  glFormat.setSampleBuffers(true);

  QVBoxLayout *layout = new QVBoxLayout;
  viewer = new Viewer(glFormat, std::move(rootNode), this);
  layout->addWidget(viewer);
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  createActions();
  createMenu();
}

void AppWindow::keyPressEvent(QKeyEvent* event) {
  int key = event->key();
  // Trigger menu actions
  auto it = shortcutActions.find(key);
  if (it != shortcutActions.end()) {
    it->second->trigger();
    return;
  }
  // Other handlers?
}

void AppWindow::createActions() {
  newMenuAction("&Quit", nullptr, "Exit the program",
      quitMenuActions, Qt::Key_Q, [this] () {
    close();
  })->setShortcuts(QKeySequence::Quit);
}

void AppWindow::createMenu() {
  appMenu = menuBar()->addMenu(tr("&Application"));
  for (auto& action : quitMenuActions) {
    appMenu->addAction(action);
  }

  modeMenu = menuBar()->addMenu(tr("&Mode"));
  for (auto& action : modeMenuActions) {
    modeMenu->addAction(action);
  }
}

QAction* AppWindow::newMenuAction(
    const std::string& title,
    QActionGroup* actionGroup,
    const std::string& tip,
    std::list<QAction*>& menuList,
    int shortcut,
    const std::function<void()> onTrigger) {

  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  if (actionGroup) {
    action->setCheckable(true);
    action->setActionGroup(actionGroup);
  }

  connect(action, &QAction::triggered, this, onTrigger);
  shortcutActions.insert({shortcut, action});
  menuList.push_back(action);

  return action;
}
