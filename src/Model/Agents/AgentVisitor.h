#ifndef AGENTVISITOR_H
#define AGENTVISITOR_H

namespace sim {

class Prey;
class Predator;

class AgentVisitor {
 public:
  AgentVisitor();
  virtual ~AgentVisitor() = default;

  virtual void visit(Prey& prey);
  virtual void visit(Prey& prey) const;

  virtual void visit(Predator& predator);
  virtual void visit(Predator& predator) const;
};

}  // namespace sim

#endif  // AGENTVISITOR_H
