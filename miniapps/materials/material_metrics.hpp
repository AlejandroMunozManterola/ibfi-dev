#ifndef MATERIAL_METRICS_HPP
#define MATERIAL_METRICS_HPP

#include "mfem.hpp"
#include <random>
#include <vector>

namespace mfem {

/// Class that implements an edge defined by a start and end point.
class Edge {
 public:
   Edge(const Vector& start, const Vector& end) : start_(start), end_(end) {}

   /// Compute the distance between a point and the edge.
   double GetDistanceTo(const Vector& x) const;

 private:
   const Vector& start_;
   const Vector& end_;
};

/// Virtual class to define the interface for defining the material topology.
class MaterialTopology {
  public:
    virtual ~MaterialTopology() = default;

    /// Compute the metric rho describing the material topology.
    virtual double ComputeMetric(const Vector &x) = 0;
};

/// Class that implements the particle topology.
class ParticleTopology : public MaterialTopology {
 public:
   /// Constructor. 
   /// @param[in]  (length_x, length_y, length_z) - particle shape
   /// @param[in]  random_positions - vector with random positions for partices
   /// @param[in]  random_rotations - vector with random rotations for particles
   ParticleTopology(double length_x, double length_y, double length_z, 
                          std::vector<double> &random_positions, 
                          std::vector<double> &random_rotations)
                          : number_of_particles_(random_positions.size() / 3), 
                            particle_shape_({length_x,length_y,length_z}) {
      Initialize(random_positions, random_rotations);
    }

   /// Compute the metric rho describing the particle topology. For a vector x,
   /// this function returns the shortest distance to any of the particles. The 
   /// individual is computed as || A_k (x-x_k) ||_2. (A allows do distort the 
   /// particle shape.)
   double ComputeMetric(const Vector &x) final;

 private:
   /// Initialize the particle topology with positions x_k and matrices A_k.
   void Initialize(std::vector<double> &random_positions, 
                   std::vector<double> &random_rotations);

   std::vector<Vector> particle_positions_; // A_k * x_k
   std::vector<DenseMatrix> particle_orientations_; // Random rotations of shape
   Vector particle_shape_; // The shape of the particle.
   int number_of_particles_; // The number of particles.
};

/// Class for the topology of a an octet truss. This class assumes the domain is
/// a cube [0,1]^3.
class OctetTrussTopology : public MaterialTopology {
 public:
   OctetTrussTopology() {Initialize();}

   // Compute the distance, i.e. distance to the closest edge.
   double ComputeMetric(const Vector &x) final;

 private:
   /// Initialize the topology, e.g. define the edges.
   void Initialize();

   /// To account for the periodicity, this function creates ghost points for 
   /// the distance computation, e.g. ( x[0] ± 1, x[1] ± 1, x[2] ± 1).
   void CreatePeriodicPoints(const Vector&x, 
                             std::vector<Vector>& periodic_points);

   std::vector<Vector> points_; // The points of the octet truss.
   std::vector<Edge> edges_;    // The edges of the octet truss.
};

} // namespace mfem

#endif // MATERIAL_METRICS_HPP
