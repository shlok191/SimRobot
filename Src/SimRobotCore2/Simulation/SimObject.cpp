/**
 * @file Simulation/SimObject.cpp
 * Implementation of class SimObject
 * @author Colin Graf
 */

#include "SimObject.h"
#include "CoreModule.h"
#include "SimObjectRenderer.h"
#include "SimObjectWidget.h"
#include <vector>
#ifdef __GNUC__
#include <cctype>
#else
#include <cstring>
#endif
#include <typeinfo>

SimObject::~SimObject()
{
  delete rotation;
  delete translation;
}

void SimObject::addParent(Element &parent)
{
  dynamic_cast<SimObject *>(&parent)->children.push_back(this);
}

void SimObject::registerObjects()
{
  for (SimObject *simObject : children)
  {
    if (simObject->name.empty())
    {
      const char *typeName = typeid(*simObject).name();
#ifdef __GNUC__
      while (std::isdigit(*typeName))
        ++typeName;
#else
      const char *str = std::strchr(typeName, ' ');
      if (str)
        typeName = str + 1;
#endif
      simObject->fullName = fullName + "." + typeName;
    }
    else
      simObject->fullName = fullName + "." + simObject->name.c_str();
    CoreModule::application->registerObject(*CoreModule::module, dynamic_cast<SimRobot::Object &>(*simObject), dynamic_cast<SimRobot::Object *>(this));
    simObject->registerObjects();
  }
}

void SimObject::registerObjects(float r_x, float r_z, float b_x, float b_z)
{
  for (SimObject *simObject : children)
  {
    if (simObject->name.empty())
    {
      const char *typeName = typeid(*simObject).name();
#ifdef WINDOWS
      const char *str = std::strchr(typeName, ' ');
      if (str)
        typeName = str + 1;
#else
      while (*typeName >= '0' && *typeName <= '9')
        ++typeName;
#endif
      simObject->fullName = fullName + "." + typeName;
    }

    else
      simObject->fullName = fullName + "." + QString::fromStdString(simObject->name);

    // Adding custom code to update the robot position to a custom value!
    if (simObject->name == "robot5")
      simObject->poseInParent.translate(r_x, 0.0f, r_z);

    // Adding custom code to update the ball position to a custom value!
    if (simObject->name == "ball")
      simObject->poseInParent.translate(b_x, 0.0f, b_z);

    CoreModule::application->registerObject(*CoreModule::module, dynamic_cast<SimRobot::Object &>(*simObject), dynamic_cast<SimRobot::Object *>(this));
    simObject->registerObjects();
  }
}

SimRobot::Widget *SimObject::createWidget()
{
  return new SimObjectWidget(*this);
}

const QIcon *SimObject::getIcon() const
{
  return &CoreModule::module->objectIcon;
}

SimRobotCore2::Renderer *SimObject::createRenderer()
{
  return new SimObjectRenderer(*this);
}
