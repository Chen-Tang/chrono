#ifndef CHMATTERMESHLESS_H
#define CHMATTERMESHLESS_H

//////////////////////////////////////////////////
//
//   ChMatterMeshless.h
//
//   Class for clusters of nodes that can 
//   simulate a visco-elasto-plastic deformable solid 
//   using the approach in Mueller ("Point based.." paper)
//   that is with a 'meshless' FEM approach.
//
//   HEADER file for CHRONO,
//	 Multibody dynamics engine
//
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////


#include <math.h>

#include "physics/ChIndexedNodes.h"
#include "physics/ChContinuumMaterial.h"
#include "collision/ChCCollisionModel.h"
#include "lcp/ChLcpVariablesNode.h"


namespace chrono
{

using namespace collision;



// Forward references (for parent hierarchy pointer)

class ChSystem;


/// Class for a single node in the meshless FEM  cluster

class ChNodeMeshless : public ChNodeBase  
{
public:
	ChNodeMeshless();
	~ChNodeMeshless();

	ChNodeMeshless (const ChNodeMeshless& other); // Copy constructor
	ChNodeMeshless& operator= (const ChNodeMeshless& other); //Assignment operator

					//
					// FUNCTIONS
					//

			// Reference (initial) position of the node - in absolute csys.
			// Note that the simulation algorithm might reset this once in a while, exp. for highly plastic objects.
	ChVector<> GetPosReference() {return pos_ref;}
			// Reference (initial) position of the node - in absolute csys. 
	void SetPosReference(const ChVector<>& mpos) {pos_ref = mpos;}

			// Get the kernel radius (max. radius while checking surrounding particles)
	double GetKernelRadius() {return h_rad;}
	void SetKernelRadius(double mr);

			// Set collision radius (for colliding with bodies, boundaries, etc.)
	double GetCollisionRadius() {return coll_rad;}
	void SetCollisionRadius(double mr);

			// Set the mass of the node
	void SetMass(double mmass) {this->variables.SetNodeMass(mmass);}
			// Get the mass of the node
	double GetMass() const {return variables.GetNodeMass();}

			// Access the 'LCP variables' of the node
	ChLcpVariables& Variables() {return variables;}

					//
					// DATA
					// 
	ChVector<> pos_ref; 
	
	ChMatrix33<> Amoment;
	ChMatrix33<> J;
	ChVector<>   m_v; 

	ChStrainTensor<> t_strain; // total strain = elastic strain + plastic strain
	ChStrainTensor<> p_strain; // plastic strain
	ChStressTensor<> t_stress; // stress

	ChLcpVariablesNode	variables;
	ChCollisionModel*	collision_model;

	ChVector<> UserForce;		

	double volume; 
	double density;
	double h_rad;
	double coll_rad;
};



/// Class for clusters of nodes that can 
/// simulate a visco-elasto-plastic deformable solid 
/// using the approach in Mueller ("Point based.." 2004 paper)
/// that is with a 'meshless' FEM approach.

class ChMatterMeshless : public ChIndexedNodes
{
						// Chrono simulation of RTTI, needed for serialization
	CH_RTTI(ChMatterMeshless,ChIndexedNodes);

private:
			//
	  		// DATA
			//
	
						// The nodes: 
	std::vector<ChNodeMeshless*> nodes;				

	ChContinuumElastoplastic material;
	
	double viscosity;

	bool do_collide;

public:

			//
	  		// CONSTRUCTORS
			//

				/// Build a cluster of nodes for Meshless and meshless FEM.
				/// By default the cluster will contain 0 particles.
	ChMatterMeshless ();

				/// Destructor
	~ChMatterMeshless ();

				/// Copy from another ChMatterMeshless. 
	void Copy(ChMatterMeshless* source);


			//
	  		// FLAGS
			//


				/// Enable/disable the collision for this cluster of particles.
				/// After setting ON, remember RecomputeCollisionModel()
				/// before anim starts (it is not automatically
				/// recomputed here because of performance issues.)
	void  SetCollide (bool mcoll);
	bool  GetCollide() {return do_collide;}


			//
	  		// FUNCTIONS
			//

				/// Get the number of nodes
	unsigned int GetNnodes() {return nodes.size();}

				/// Access the N-th node 
	ChNodeBase& GetNode(unsigned int n) { assert(n<nodes.size()); return *nodes[n];}
				
				/// Resize the node cluster. Also clear the state of 
				/// previously created particles, if any.
	void ResizeNnodes(int newsize);

				/// Add a new node to the particle cluster, passing a 
				/// vector as initial position.
	void AddNode(ChVector<double> initial_state);



		//
		// LCP INTERFACE
		//

			 // Override/implement LCP system functions of ChPhysicsItem
			 // (to assembly/manage data for LCP system solver)

	void VariablesFbReset();

	void VariablesFbLoadForces(double factor=1.);

	void VariablesQbLoadSpeed();

	void VariablesQbSetSpeed(double step=0.);

	void VariablesQbIncrementPosition(double step);

	virtual void InjectVariables(ChLcpSystemDescriptor& mdescriptor);



			   // Other functions

				/// Set no speed and no accelerations (but does not change the position)
	void SetNoSpeedNoAcceleration();

			
				/// Synchronize coll.models coordinates and bounding boxes to the positions of the particles.
	virtual void SyncCollisionModels();
	virtual void AddCollisionModelsToSystem();
	virtual void RemoveCollisionModelsFromSystem();

	void UpdateParticleCollisionModels();


				/// Access the material
	ChContinuumElastoplastic&  GetMaterial() {return material;}
	
	void SetViscosity(double mvisc) { viscosity=mvisc;}
	double GetViscosity() {return viscosity;}


			//
			// UPDATE FUNCTIONS
			//

				/// Update all auxiliary data of the particles 
	virtual void Update (double mytime);
				/// Update all auxiliary data of the particles
	virtual void Update ();

				/// Tells to the associated external object ChExternalObject() ,if any,
				/// that its 3D shape must be updated in order to syncronize to ChBody
				/// coordinates
	void UpdateExternalGeometry ();


			//
			// STREAMING
			//


				/// Method to allow deserializing a persistent binary archive (ex: a file)
				/// into transient data.
	void StreamIN(ChStreamInBinary& mstream);

				/// Method to allow serializing transient data into a persistent
				/// binary archive (ex: a file).
	void StreamOUT(ChStreamOutBinary& mstream);


};



// A shortcut..
typedef ChSharedPtr<ChMatterMeshless> ChSharedMatterMeshlessPtr;



} // END_OF_NAMESPACE____


#endif