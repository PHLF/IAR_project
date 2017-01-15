#ifndef RETINA_H
#define RETINA_H

#include "../Environnement/Environment.h"

class Retina {
 public:
  Retina(uint32_t seg, uint32_t fov);
  virtual ~Retina();
  uint32_t getNbSegments() const { return _nbSegments; }

 protected:
 private:
  std::shared_ptr<Environment> _env;
  uint32_t _nbSegments;
  uint32_t _fieldOfView;  // Angle in °
  uint32_t _depth;
};

#endif  // RETINA_H
