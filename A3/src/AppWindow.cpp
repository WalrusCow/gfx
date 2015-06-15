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
  newMenuAction("Reset Position", nullptr, "Reset puppet position",
      quitMenuActions, Qt::Key_I, [this] () {
    // TODO: Reset puppet position
  });

  newMenuAction("Reset Orientation", nullptr, "Reset puppet orientation",
      quitMenuActions, Qt::Key_O, [this] () {
    // TODO: Reset puppet orientation
  });

  newMenuAction("Reset Joints", nullptr, "Reset joints and undo/redo stack",
      quitMenuActions, Qt::Key_N, [this] () {
    // TODO: Reset joints
  });

  newMenuAction("Reset All", nullptr, "Reset joints and puppet",
      quitMenuActions, Qt::Key_A, [this] () {
    // TODO: Reset all
  });

  newMenuAction("&Quit", nullptr, "Exit the program",
      quitMenuActions, Qt::Key_Q, [this] () {
    close();
  })->setShortcuts(QKeySequence::Quit);

  QActionGroup* modeGroup = new QActionGroup(this);
  newMenuAction("&Position/Orientation", modeGroup, "Move or rotate the puppet",
      modeMenuActions, Qt::Key_P, [this] () {
    // TODO: Enable position/orientation mode
  });
  modeGroup->actions().first()->setChecked(true);

  newMenuAction("&Joints", modeGroup, "Select and rotate joints",
      modeMenuActions, Qt::Key_J, [this] () {
      // TODO: Enable Joint mode
  });

  newMenuAction("&Undo", nullptr, "Undo last action",
      editMenuActions, Qt::Key_U, [this] () {
    // TODO: Undo
  });

  newMenuAction("&Redo", nullptr, "Redo last action",
      editMenuActions, Qt::Key_R, [this] () {
    // TODO: redo
  });

  newMenuAction("&Circle", nullptr, "Draw circle for trackball",
      optionsMenuActions, Qt::Key_C, [this] () {
    // TODO
  })->setCheckable(true);

  newMenuAction("&Z-Buffer", nullptr, "Draw puppet with Z-buffer",
      optionsMenuActions, Qt::Key_Z, [this] () {
    // TODO
  })->setCheckable(true);

  newMenuAction("&Backface Cull", nullptr, "Remove backfacing polygons",
      optionsMenuActions, Qt::Key_B, [this] () {
    // TODO
  })->setCheckable(true);

  newMenuAction("&Frontface Cull", nullptr, "Remove frontfacing polygons",
      optionsMenuActions, Qt::Key_F, [this] () {
    // TODO
  })->setCheckable(true);
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

  editMenu = menuBar()->addMenu(tr("&Edit"));
  for (auto& action : editMenuActions) {
    editMenu->addAction(action);
  }

  optionsMenu = menuBar()->addMenu(tr("&Options"));
  for (auto& action : optionsMenuActions) {
    optionsMenu->addAction(action);
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
