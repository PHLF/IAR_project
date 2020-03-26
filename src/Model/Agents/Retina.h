#ifndef RETINA_H
#define RETINA_H

#include <cstdint>
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

  virtual Span<LayerBase> layers() = 0;

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
      : RetinaBase(nb_segments, los, fov, ref_position), _layers({([this] {
          std::vector<Cell> cells;

          for (size_t i = 0; i < _nb_segments; ++i) {
            Cell cell{nullptr, _view_vectors[i], _view_vectors[i + 1]};
            cells.emplace_back(std::move(cell));
          }
          return new Layer<StimulusType>(_ref_position, cells, _los);
        }())...}) {}

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

  Span<LayerBase> layers() override { return {_layers.data(), _layers.size()}; }

 private:
  std::array<std::unique_ptr<LayerBase>, sizeof...(StimulusType)> _layers;
};

}  // namespace sim

#endif  // RETINA_H
