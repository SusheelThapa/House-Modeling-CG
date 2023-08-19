#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "Bone.h"
#include <functional>
#include "animdata.h"
#include "model.h"

/**
 * Custom Data Type: AssimpNodeData
 *
 * It is used to represent information about nodes in the Assimp scene hierarchy
 */
struct AssimpNodeData
{
	glm::mat4 transformation;			  // Represent transformation matrix
	std::string name;					  // Holds the name of the node
	int childrenCount;					  // Holds the count of child nodes
	std::vector<AssimpNodeData> children; // Vector that holds the instance of AssimpNodeData
};

class Animation
{
public:
	/*Default constructor of Animation Class*/
	Animation() = default;

	/**
	 * Constructor responsible for reading animation data from file,
	 * setting some animation properties,
	 * and initializing the  hierarchy and bone related to the animation
	 */
	Animation(const std::string &animationPath, Model *model)
	{
		/*Object created to read the animation file*/
		Assimp::Importer importer;

		/*Reading the animation files*/
		const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

		/*Checking if the scene and its root nodde are valid or not*/
		assert(scene && scene->mRootNode);

		/*Extracting the first animation from the scene*/
		auto animation = scene->mAnimations[0];

		/*Setting the duration of animation and ticks per second(no. of animation keyframe per second)*/
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;

		/*Extracting transformation matrix of the root node*/
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;

		/*Inverting to bring to the local space*/
		globalTransformation = globalTransformation.Inverse();

		/*Reading hierarchy data of animation, starting from the root node of the scene.*/
		ReadHeirarchyData(m_RootNode, scene->mRootNode);

		/*Reading and populating bone-related data for animation*/
		ReadMissingBones(animation, *model);
	}

	/**
	 * Searches for a bone with a given name within the list of bones
	 * stored in the 'm_Bones' vector of animations
	 */
	Bone *FindBone(const std::string &name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
								 [&](const Bone &Bone)
								 {
									 return Bone.GetBoneName() == name;
								 });

		if (iter == m_Bones.end())
			return nullptr;
		else
			return &(*iter);
	}

	/*Returns the number of animation ticks per second*/
	inline float GetTicksPerSecond() { return m_TicksPerSecond; }

	/*Returns the duration of the animation in ticks*/
	inline float GetDuration() { return m_Duration; }

	/*Return a reference to the root node of the animation's hierarchy*/
	inline const AssimpNodeData &GetRootNode() { return m_RootNode; }

	/*Returns a reference to a map that stores bone information*/
	inline const std::map<std::string, BoneInfo> &GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}

private:
	/**
	 * Reading bone information from animation channels and
	 * adding any missing bones to the bone information map.
	 */
	void ReadMissingBones(const aiAnimation *animation, Model &model)
	{
		/*Retrieves the number of animation channels*/
		int size = animation->mNumChannels;

		/*Retrieve the reference to the bone information map from 'model' object*/
		auto &boneInfoMap = model.GetBoneInfoMap(); // getting m_BoneInfoMap from Model class
		int &boneCount = model.GetBoneCount();		// getting the m_BoneCounter from Model class

		/*Reading channels(bones engaged in an animation and their keyframes)*/
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}

			m_Bones.push_back(Bone(channel->mNodeName.data,
								   boneInfoMap[channel->mNodeName.data].id, channel));
		}

		/*Update boneInfoMap*/
		m_BoneInfoMap = boneInfoMap;
	}

	/**
	 * Recursively reading the hierarchy data of nodes in the animation's scene
	 * and populating an 'AssimpNodeData' structure, that structure represents
	 * the hierarchical structure of nodes in the scene.
	 */
	void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src)
	{
		/*Assure that node pointer is not null*/
		assert(src);

		/*Copies name of source node, transformation matrix, and set the number of children node*/
		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		/*Recursively iterated through each child node of source node*/
		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}

	/*Variable to store information about animation, including bode data, hierarchy structure and other animation-related details*/
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};
