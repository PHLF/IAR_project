#include "AgentVisitor.h"

sim::AgentVisitor::AgentVisitor() {}

void sim::AgentVisitor::visit(sim::Prey& /* prey */) {}

void sim::AgentVisitor::visit(sim::Prey& /* prey */) const {}

void sim::AgentVisitor::visit(sim::Predator& /* predator */) {}

void sim::AgentVisitor::visit(sim::Predator& /* predator */) const {}
