#ifndef RETINA_H
#define RETINA_H

#include <cstdint>
#include <utility>
#include <vector>

#include "Misc/Utils.h"
#include "layer.h"

namespace sim {

class Agent;

class RetinaBase {
 public:
  RetinaBase(uint32_t nb_segments,
             uint32_t los,
             uint32_t fov,
             const Coords& ref_position);
  virtual ~RetinaBase();

  virtual void see(Agent& agent) = 0;

  virtual void clear() = 0;

  virtual Span<const std::unique_ptr<LayerBase>> layers() const = 0;

  void update_view_vectors(uint32_t orientation);
  const std::vector<Coords>& view_vectors() const;

 protected:
  const Coords& _ref_position;
  uint32_t _nb_segments;
  ffloat _los;

  std::vector<uint32_t> _theta_i;
  std::vector<Coords> _view_vectors;
};

template <typename... StimulusType>
class Retina : public RetinaBase {
 public:
  Retina(uint32_t nb_segments,
         uint32_t los,
         uint32_t fov,
         const Coords& ref_position)
      : RetinaBase(nb_segments, los, fov, ref_position),
        _layers({std::make_unique<Layer<StimulusType>>(_ref_position,
                                                       _view_vectors,
                                                       los)...}) {}

  void see(Agent& agent) override {
    for (auto& layer : _layers) {
      agent.accept(*layer);
    }
  }

  void clear() override {
    for (auto& layer : _layers) {
      layer->clear();
    }
  }

  Span<const std::unique_ptr<LayerBase>> layers() const override {
    return {_layers.data(), _layers.size()};
  }

 private:
  std::array<std::unique_ptr<LayerBase>, sizeof...(StimulusType)> _layers;
};

}  // namespace sim

#endif  // RETINA_H
