#pragma once

#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp_glm_helpers.h>
#include <vector>

/**
 * Custom Data Types:
 *
 * KeyPosition: Represents a 3D position keyframe in an animation sequence
 *
 * KeyRotation: Represents a quaternion-based rotation keyframe in an animation sequence.
 *
 * KeyScale: Represents a 3D scale keyframe in an animation sequence.
 */
struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	/**
	 * Constructor of Bone Class:
	 *
	 * This class takes various parameter to initialize the bone's
	 * properties using animation data from an 'aiNodeAnim' object.
	 */
	Bone(const std::string &name, int ID, const aiNodeAnim *channel)
		: m_Name(name),
		  m_ID(ID),
		  m_LocalTransform(1.0f)
	{
		/*Stores the number of position keyframes in the animation channel*/
		m_NumPositions = channel->mNumPositionKeys;

		/*Iterating over position keyframes*/
		for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
		{
			/*Extracting the position and timestamp of each position keyframe from the animation channel*/
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;

			/*Convert the AIVector3d position to a glm::vec3 using AssimpGLMHelpers*/
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;

			/*Store the position and timestamp in KeyPosition struct and add it to vector*/
			m_Positions.push_back(data);
		}

		/*Stores the number of rotation keyframes in the animation channel*/
		m_NumRotations = channel->mNumRotationKeys;

		/*Iterating over rotation keyframes*/
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			/*Extracting the position and timestamp of each position keyframe from the animation channel*/
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;

			/*Converts the AIQUaternion orientation to a glm::quat using AssimpGLMHelpers function*/
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;

			/*Stores the orientation and timestamp in the "KeyRotation" struct and adds it to the vector*/
			m_Rotations.push_back(data);
		}

		/*Stores the number of scaling keyframes in the animation channel*/
		m_NumScalings = channel->mNumScalingKeys;

		/*Iterating over scaling keyframes*/
		for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			/*Extracts the scaling and timestamp of each scaling keyframe from the animation channel*/
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;

			/*Converts the AIScalar scale to glm:vec3 using AsssimpGMLHelpers*/
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(scale);
			data.timeStamp = timeStamp;

			/*Adding it to the vector*/
			m_Scales.push_back(data);
		}
	}

	/**
	 * Updating the local transformation matrix of the bone
	 * based on the animation time by calculating interpolated translation,
	 * rotation and scaling matrices based on animation time..
	 */
	void Update(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScaling(animationTime);

		m_LocalTransform = translation * rotation * scale;
	}

	/*Returns the local transformation matrix of bone*/
	glm::mat4 GetLocalTransform() { return m_LocalTransform; }

	/*Return the name of the bone*/
	std::string GetBoneName() const { return m_Name; }

	/*Returns the id of the bone*/
	int GetBoneID() { return m_ID; }

	/*Returns the index of the keyframe position within 'm_Positions' vector*/
	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	/*Returns the index of the keyframe position within 'm_Rotation' vector */
	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	/*Returns the index of keyframe position within 'm_NumScalings' vecctor*/
	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

private:
	/*Calculates the interpolation factor for scaling keyframes*/
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;

		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;

		scaleFactor = midWayLength / framesDiff;

		return scaleFactor;
	}

	/**
	 * Perform position interpolation for bone transformation
	 * based on the provided animation time
	 */
	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (1 == m_NumPositions)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;

		float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
										   m_Positions[p1Index].timeStamp, animationTime);

		glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);

		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	/**
	 * Performs rotation interpolation for bone transformation
	 * based on the provided animation time
	 */
	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (1 == m_NumRotations)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;

		float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
										   m_Rotations[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);

		finalRotation = glm::normalize(finalRotation);

		return glm::toMat4(finalRotation);
	}

	/**
	 * Performs scaling interpolation for bone transformations
	 * based on the provided animation time
	 */
	glm::mat4 InterpolateScaling(float animationTime)
	{
		if (1 == m_NumScalings)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
										   m_Scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	/*Member variable of Bone Class*/
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;
};
