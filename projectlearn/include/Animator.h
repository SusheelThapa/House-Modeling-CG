#pragma once
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"
#include "Bone.h"

class Animator
{
public:
	/**
	 * Initializes the Animator objects with an animation,
	 * sets the initial time to 0, preallocates memory for storing
	 * bone transformation matrices.
	 */
	Animator(Animation *animation)

	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		/*Reserve memory to hold 1000 elements*/
		m_FinalBoneMatrices.reserve(1000);

		/*Fills m_FinalBoneMatrices vector with identity matrices*/

		for (int i = 0; i < 1000; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	/*Updates the animation based on the given time increment*/
	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	/*Sets the animator to play a new animation*/
	void PlayAnimation(Animation *pAnimation)

	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	/**
	 * Recursively calculates the transformation matrices of bones
	 * in the skeletal hierarchy based on the current animation time.
	 * */
	void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)
	{
		/*Retrieves the name of current node*/
		std::string nodeName = node->name;
		/*Retrieves the transformation matrix of current node*/
		glm::mat4 nodeTransform = node->transformation;

		/*Trying to find the Bone with same name as the current node in the current animation*/
		Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

		/*If respective bone is found or not*/
		if (Bone)
		{
			/*Update bone transformation based on the current animation time.*/
			Bone->Update(m_CurrentTime);

			/*Retrieves the local transformation matrix of the matrix*/
			nodeTransform = Bone->GetLocalTransform();
		}

		/**
		 * Calculates the global transformation matrix for the current node
		 * by combining the parent transformation with the node transformation
		 */
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		/*Retrieve the map of bone information from the current animation*/
		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

		/*Checks if the current node's name exists in the bone information map.*/
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			/*Retrieve bone index*/
			int index = boneInfoMap[nodeName].id;

			/*Retrieve bone offset matrix*/
			glm::mat4 offset = boneInfoMap[nodeName].offset;

			/*Calculate and assign the final transformation matrix for the bone*/
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		/*Iterates through the children of the current node*/
		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}


	/*Return the vector of final bone transformation matrices that were calculated during animation update*/
	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation *m_CurrentAnimation = nullptr;
	float m_CurrentTime = 0.0f;
	float m_DeltaTime = 0.0f;
};