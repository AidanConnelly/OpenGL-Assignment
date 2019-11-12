//
// Created by m on 14/10/2019.
//

#include "daeParser.h"
#include "stringToFloatFast.h"
#include "xmlNode.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "XMLParseState.h"
#include "../../../src/Vertex.h"

//#include <glm/gtc/type_ptr.hpp>

void daeParser::parseNodeTagNames(std::vector<char>& buffer, xmlNodeStore& nodes)
{
	alterXmlNodes(nodes, [&](xmlNode* node) -> xmlNode
	{
		int index = node->startIndex + 1;
		char character = buffer[index];
		while (character != ' ' && character != '/' && character != '>')
		{
			character = buffer[++index];
		}
		std::string tagName(buffer.begin() + node->startIndex + 1, buffer.begin() + index);
		node->tagName = tagName;
		return *node;
	});
}

std::vector<MeshData> daeParser::parse(std::vector<char>& buffer, std::string directory)
{
	xmlNodeStore nodes = parseNodes(buffer);
	parseNodeTagNames(buffer, nodes);
	xmlNodeVector asVec;
	std::for_each(nodes.begin(), nodes.end(), [&](xmlNode* node) -> void
	              {
		              //todo
		              asVec.push_back(*node);
	              }
	);
	bufferParseResult result = parseLargeBuffers(nodes);
	int x = sizeof(result.floatArrays[0]);
	auto nodeParseResults = parseNodeTags(buffer, asVec);
	auto meshParseResults = parseMeshTags(buffer, asVec, &result);
	//todo apply transforms to instance_gemetries???
	std::cout << result.floatArrays.size();
	std::vector<MeshData> toReturn;
	for (auto& a : meshParseResults)
	{
		std::vector<std::string> textures;
		for (auto& b : a.textureIds)
		{
			std::string fileName = getFileNameFromMaterialID(nodes, b);
			if (fileName != "")
			{
				std::string fullFileName = directory + fileName;
				textures.push_back(fullFileName);
			}
		}
		toReturn.push_back(MeshData(a.vertexes, a.triangles, textures));
	}
	return toReturn;
}

std::string daeParser::getFileNameFromMaterialID(xmlNodeStore nodes, std::string materialId)
{
	xmlNode* library_materials = getSoleByTag(nodes, "library_materials");
	xmlNode material = getSoleByAttrib(library_materials->children, "id", materialId);
	xmlNode* instance_effect = getSoleByTag(material.children, "instance_effect");

	xmlNode* library_effects = getSoleByTag(nodes, "library_effects");
	std::string idOfEffect = removeLeadingHash(instance_effect->getAttribute("url"));
	xmlNode effect = getSoleByAttrib(library_effects->children, "id", idOfEffect);
	xmlNode* profile_common = getSoleByTag(effect.children, "profile_COMMON");
	xmlNode* technique = getSoleByTag(profile_common->children, "technique");
	xmlNode* lambert = getSoleByTag(technique->children, "lambert");
	xmlNode* diffuse = getSoleByTag(lambert->children, "diffuse");
	if (filterByTagName(diffuse->children, "texture").size() == 1)
	{
		xmlNode* texture = getSoleByTag(diffuse->children, "texture");
		std::string sampler2DSid = texture->getAttribute("texture");

		xmlNode newParamSampler = getSoleByAttrib(profile_common->children, "sid", sampler2DSid);
		xmlNode* sampler2d = getSoleByTag(newParamSampler.children, "sampler2D");
		xmlNode* source = getSoleByTag(sampler2d->children, "source");
		std::string surfaceSid = source->getContents();

		xmlNode newParamSurface = getSoleByAttrib(profile_common->children, "sid", surfaceSid);
		xmlNode* surface = getSoleByTag(newParamSurface.children, "surface");
		xmlNode* surface_init_from = getSoleByTag(surface->children, "init_from");
		std::string imageId = surface_init_from->getContents();

		xmlNode* library_images = getSoleByTag(nodes, "library_images");
		xmlNode image = getSoleByAttrib(library_images->children, "id", imageId);
		xmlNode* init_from = getSoleByTag(image.children, "init_from");
		return init_from->getContents();
	}
	return "";
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-use-emplace"

std::vector<parseNodeTagsResult> daeParser::parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes)
{
	std::vector<parseNodeTagsResult> toReturn;
	auto nodeTags = filterByTagName(nodes, "node");
	mapXmlNodes(nodeTags, [&](xmlNode node) -> xmlNode
	{
		std::vector<xmlNode *> children = node.children;
		auto i_g = filterByTagName(children, "instance_geometry");
		if (filterByTagName(children, "matrix").size() == 1)
		{
			auto matrix = getSoleByTag(children, "matrix");
			toReturn.push_back(parseNodeTagsResult(*matrix, i_g));
		}
		else
		{
			toReturn.push_back(parseNodeTagsResult(i_g));
		}
		return node;
	});
	return toReturn;
}

#pragma clang diagnostic pop

std::vector<meshParseResult> daeParser::parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes,
                                                      bufferParseResult* largeBuffers)
{
	std::vector<meshParseResult> results;
	std::vector<xmlNode> tags = filterByTagName(nodes, "geometry");
	for (auto& geometryTag : tags)
	{
		meshParseResult thisResult = meshParseResult();
		std::string id = geometryTag.getAttribute("id");
		xmlNode tag = *geometryTag.children[0];
		if (tag.tagName != "mesh")
		{
			throw std::invalid_argument("unexpected state");
		}
		thisResult.meshID = id;
		xmlNodeStore triangles = filterByTagName(tag.children, "triangles");
		for (auto& triangleTagPtr : triangles)
		{
			if (triangleTagPtr->hasAttribute("material"))
			{
				thisResult.textureIds.push_back(triangleTagPtr->getAttribute("material"));
			}
			auto triangleTag = *triangleTagPtr;
			xmlNode pTag = *getSoleByTag(triangleTag.children, "p");
			auto vertexInputTag = getSoleByAttrib(triangleTag.children, "semantic", "VERTEX");
			auto normalInputTag = getSoleByAttrib(triangleTag.children, "semantic", "NORMAL");
			int vertexOffset = stoi(vertexInputTag.getAttribute("offset"));
			int normalOffset = stoi(normalInputTag.getAttribute("offset"));
			int maxOffset = 0;
			auto inputTags = filterByTagName(triangleTag.children, "input");
			for (auto& nodePtr : inputTags)
			{
				auto node = *nodePtr;
				int offset = std::stoi(node.getAttribute("offset"));
				if (offset > maxOffset)
				{
					maxOffset = offset;
				}
			}

			xmlNode* thisTriangleIndexArray = nullptr;
			for (auto& indexArray : largeBuffers->indexArrays)
			{
				if (indexArray->id == pTag.id)
				{
					thisTriangleIndexArray = indexArray;
				}
			}

			if (thisTriangleIndexArray == nullptr)
			{
				throw std::invalid_argument("todo");
			}

			int numberOfIndexes = thisTriangleIndexArray->indexesIfApplicable.size();
			int vertexCount = numberOfIndexes / (maxOffset + 1);
			int triangleCount = vertexCount / 3;

			int expectedNumberOfIndexes = triangleCount * (maxOffset + 1) * 3;
			if (expectedNumberOfIndexes != numberOfIndexes)
			{
				throw std::invalid_argument("wrong number of indexes");
			}

			const std::string& verticesID = vertexInputTag.getAttribute("source");
			auto vertexTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(verticesID));
			const std::string& inputID = vertexTag.children[0]->getAttribute("source");
			auto positionSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(inputID));

			const std::string& normalSourceId = normalInputTag.getAttribute("source");
			auto normalSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(normalSourceId));

			struct textureCoordinateData
			{
				xmlNode* ar;
				paramInfo s;
				paramInfo t;
			};

			textureCoordinateData* tData = NULL;
			int tCoordOffset = 0;
			if (anyByAttrib(triangleTag.children, "semantic", "TEXCOORD")) {
				auto tCoordInputTag = getSoleByAttrib(triangleTag.children, "semantic", "TEXCOORD");
				tCoordOffset = stoi(tCoordInputTag.getAttribute("offset"));
				const std::string& tCoordSourceId = tCoordInputTag.getAttribute("source");
				auto tCoordSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(tCoordSourceId));

				xmlNode* tCoordArray = soleLargeFloatChild(tCoordSourceTag, largeBuffers);
				paramInfo sParam = prepareParam("S", &tCoordSourceTag, tCoordArray);
				paramInfo tParam = prepareParam("T", &tCoordSourceTag, tCoordArray);
				tData = new textureCoordinateData{ tCoordArray, sParam, tParam };
			}
			struct colourData
			{
				xmlNode* ar;
				paramInfo re;
				paramInfo gr;
				paramInfo bl;
			};

			colourData* cData = NULL;
			int colourOffset = 0;
			if (anyByAttrib(triangleTag.children, "semantic", "COLOR")) {
				auto colourInputTag = getSoleByAttrib(triangleTag.children, "semantic", "COLOR");
				colourOffset = stoi(colourInputTag.getAttribute("offset"));
				const std::string& colourSourceId = colourInputTag.getAttribute("source");
				auto colourSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(colourSourceId));

				xmlNode* colourArray = soleLargeFloatChild(colourSourceTag, largeBuffers);
				paramInfo rParam = prepareParam("R", &colourSourceTag, colourArray);
				paramInfo gParam = prepareParam("G", &colourSourceTag, colourArray);
				paramInfo bParam = prepareParam("B", &colourSourceTag, colourArray);
				colourData* made = new colourData{ colourArray, rParam, gParam, bParam };
				cData = made;
			}

			auto positnFloat = soleLargeFloatChild(positionSourceTag, largeBuffers);
			paramInfo xParam = prepareParam("X", &positionSourceTag, positnFloat);
			paramInfo yParam = prepareParam("Y", &positionSourceTag, positnFloat);
			paramInfo zParam = prepareParam("Z", &positionSourceTag, positnFloat);
			auto nrmlFloat = soleLargeFloatChild(normalSourceTag, largeBuffers);
			paramInfo nXParam = prepareParam("X", &normalSourceTag, nrmlFloat);
			paramInfo nYParam = prepareParam("Y", &normalSourceTag, nrmlFloat);
			paramInfo nZParam = prepareParam("Z", &normalSourceTag, nrmlFloat);

			for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				//region Region_1
				for (int i = 0; i < 3; i++)
				{
					Vertex building{};

					unsigned vertexIndex = 3 * triangleIndex + i;
					unsigned positnIndex = thisTriangleIndexArray->indexesIfApplicable[(maxOffset + 1) * vertexIndex +
						vertexOffset];
					unsigned normalIndex = thisTriangleIndexArray->indexesIfApplicable[(maxOffset + 1) * vertexIndex +
						normalOffset];
					unsigned tCoordIndex = thisTriangleIndexArray->indexesIfApplicable[(maxOffset + 1) * vertexIndex +
						tCoordOffset];
					unsigned colourIndex = thisTriangleIndexArray->indexesIfApplicable[(maxOffset + 1) * vertexIndex +
						colourOffset];

					//vertex X
					building.x = positnFloat->floatsIfApplicable[xParam.stride * positnIndex + xParam.idx];
					//vertex Y
					building.y = positnFloat->floatsIfApplicable[yParam.stride * positnIndex + yParam.idx];
					//vertex Z
					building.z = positnFloat->floatsIfApplicable[zParam.stride * positnIndex + zParam.idx];

					//vertexNormal X
					building.nX = nrmlFloat->floatsIfApplicable[nXParam.stride * normalIndex + nXParam.idx];
					//vertexNormal Y
					building.nY = nrmlFloat->floatsIfApplicable[nYParam.stride * normalIndex + nYParam.idx];
					//vertexNormal Z
					building.nZ = nrmlFloat->floatsIfApplicable[nZParam.stride * normalIndex + nZParam.idx];

					if (tData) {
						//texCoord U
						building.u = tData->ar->floatsIfApplicable[tData->s.stride * tCoordIndex + tData->s.idx];
						//texCoord V
						building.v = tData->ar->floatsIfApplicable[tData->t.stride * tCoordIndex + tData->t.idx];
					}

					if(cData)
					{
						building.r = cData->ar->floatsIfApplicable[cData->re.stride * colourIndex + cData->re.idx];
						building.g = cData->ar->floatsIfApplicable[cData->gr.stride * colourIndex + cData->gr.idx];
						building.b = cData->ar->floatsIfApplicable[cData->bl.stride * colourIndex + cData->bl.idx];
					}
					
					thisResult.vertexes.push_back(building);
				}
				Triangle constructed{};
				constructed.v1i = thisResult.vertexes.size() - 3;
				constructed.v2i = thisResult.vertexes.size() - 2;
				constructed.v3i = thisResult.vertexes.size() - 1;
				thisResult.triangles.push_back(constructed);
			}
			//endregion Region_1


			//Can now construct vertex buffer and vertex index buffer from p tag data
		}
		//region Region_2

		//Find <p>,
		//Find semantic = VERTEX <vertices> ~> <input> child ~> <source>
		//Find semantic = NORMAL <source>
		//Find semantic = TEXCOORD <source>

		//<source>
		//<float_array>
		//<technique_common>
		//From = Index of <param NUM>
		//To = NUM
		//endregion Region_2

		results.push_back(thisResult);
	}
	return results;
}

xmlNode* daeParser::soleLargeFloatChild(const xmlNode& parent, bufferParseResult* bpr)
{
	for (auto& b : parent.children)
	{
		for (auto& a : bpr->floatArrays)
		{
			unsigned aId = a->id;
			unsigned bId = b->id;
			if (aId == bId)
			{
				return a;
			}
		}
	}
	throw std::invalid_argument("no large float child found");
}

float daeParser::getViaParam(std::string toGet, xmlNode* sourceTag, xmlNode* parsedFloatArray, unsigned index)
{
	auto technique_common = getSoleByTag(sourceTag->children, "technique_common");
	auto accessor = getSoleByTag(technique_common->children, "accessor");
	int count = stoi(accessor->getAttribute("count"));
	int stride = std::stoi(accessor->getAttribute("stride"));
	int offset = 0;
	if (accessor->hasAttribute("offset"))
	{
		const std::string& asStr = accessor->getAttribute("offset");
		offset = stoi(asStr);
	}

	xmlNodeStore params = filterByTagName(accessor->children, "param");
	int idx = -1;
	for (int i = 0; i < params.size(); i++)
	{
		if (params[i]->getAttribute("name") == toGet)
		{
			idx = i + offset;
		}
	}
	if (idx == -1)
	{
		throw std::invalid_argument("todo");
	}
	return parsedFloatArray->floatsIfApplicable[index * stride + idx];
}

paramInfo daeParser::prepareParam(std::string toGet, xmlNode* sourceTag, xmlNode* parsedFloatArray)
{
	auto technique_common = getSoleByTag(sourceTag->children, "technique_common");
	auto accessor = getSoleByTag(technique_common->children, "accessor");
	int count = stoi(accessor->getAttribute("count"));
	int stride = std::stoi(accessor->getAttribute("stride"));
	int offset = 0;
	if (accessor->hasAttribute("offset"))
	{
		const std::string& asStr = accessor->getAttribute("offset");
		offset = stoi(asStr);
	}

	xmlNodeStore params = filterByTagName(accessor->children, "param");
	int idx = -1;
	for (int i = 0; i < params.size(); i++)
	{
		if (params[i]->getAttribute("name") == toGet)
		{
			idx = i + offset;
		}
	}
	if (idx == -1)
	{
		throw std::invalid_argument("todo");
	}
	return paramInfo(stride, idx);
}

xmlNode daeParser::getSoleByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value)
{
	for (auto& node : lookIn)
	{
		if (node->getAttribute(attrib) == value)
		{
			return *node;
		}
	}
	throw std::invalid_argument("no match");
}

bool daeParser::anyByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value)
{
	for (auto& node : lookIn)
	{
		if (node->getAttribute(attrib) == value)
		{
			return true;
		}
	}
	return false;
}

xmlNode* daeParser::getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor)
{
	auto x = filterByTagName(toSearchIn, toSearchFor);
	if (x.size() != 1)
	{
		throw std::invalid_argument("not single");
	}
	return x[0];
}

bufferParseResult daeParser::parseLargeBuffers(xmlNodeStore nodes)
{
	xmlNodeStore floatArrays = filterByTagName(nodes, "float_array");
	floatArrays = parseFloatArrays(nodes[0]->buffer, floatArrays);

	xmlNodeStore indexArrays = filterByTagName(nodes, "p");
	indexArrays = parseIndexBuffer(nodes[0]->buffer, indexArrays);

	bufferParseResult result(floatArrays, indexArrays);
	return result;
}

xmlNodeVector daeParser::filterByTagName(const xmlNodeVector& nodes, const std::string& tagName)
{
	xmlNodeVector toReturn;
	for (auto const& node : nodes)
	{
		if (node.tagName == tagName)
		{
			toReturn.push_back(node);
		}
	}
	return toReturn;
}

xmlNodeStore daeParser::filterByTagName(xmlNodeStore nodes, const std::string& tagName)
{
	xmlNodeStore toReturn;
	for (auto const& node : nodes)
	{
		if (node->tagName == tagName)
		{
			toReturn.push_back(node);
		}
	}
	return toReturn;
}

void parseFloatArray(xmlNode* node)
{
	int index = node->startIndex;
	while ((node->buffer)[index] != '>')
	{
		index++;
	}
	std::vector<float> floats;
	do
	{
		index++;
		float f = parseAFloat(&index, node->buffer);
		floats.push_back(f);
	}
	while (node->buffer[index] == ' ');
	node->floatsIfApplicable = floats;
}

xmlNodeStore daeParser::parseFloatArrays(std::vector<char> buffer, xmlNodeStore floatArrays)
{
	int workers = 4;
	std::vector<std::thread*> pool;
	for (int i = 0; i < floatArrays.size(); i++)
	{
		int workerNum = i % workers;
		if (pool.size() >= (workerNum + 1))
		{
			pool[workerNum]->join();
			delete pool[workerNum];
			pool[workerNum] = new std::thread(parseFloatArray, floatArrays[i]);
		}
		else
		{
			pool.push_back(new std::thread(parseFloatArray, floatArrays[i]));
		}
	}
	for (int i = 0; i < workers && i < floatArrays.size(); i++)
	{
		pool[i]->join();
		delete pool[i];
	}

	return floatArrays;
}

void daeParser::checkForQuotes(char thisChar, int* stackPos, parseStack* stack, xmlParsingStackMember* state)
{
	int newStackPos;
	switch (thisChar)
	{
	case '"':
		(*stack).push_back(*state);
		newStackPos = *stackPos + 1;
		*stackPos = newStackPos;
		(*state).state = XMLParseState::DQuote;
		break;
	case '\'':
		(*stack).push_back(*state);
		newStackPos = *stackPos + 1;
		*stackPos = newStackPos;
		(*state).state = XMLParseState::SQuote;
		break;
	default:
		break;
	}
}

xmlNodeStore daeParser::parseNodes(const std::vector<char>& buffer)
{
	xmlNodeStore nodes = {};
	//Loop over buffer
	parseStack stack = std::vector<xmlParsingStackMember>();
	auto* node = new xmlNode(buffer);
	xmlParsingStackMember state = xmlParsingStackMember(Start, node);
	int stackPos = 0;
	for (unsigned i = 0; i < buffer.size(); i++)
	{
		char thisChar = buffer[i];
		switch (state.state)
		{
		case XMLParseState::Start:
			if (thisChar == '<')
			{
				state.state = XMLParseState::TagOpened;
				state.node = new xmlNode(buffer);
				state.node->startIndex = i;
			}
			break;
		case XMLParseState::TagOpened:
			if (thisChar == '/')
			{
				state.state = XMLParseState::EndTag;
			}
			else if (thisChar == '?')
			{
				state.state = XMLParseState::Start;
			}
			else
			{
				state.state = XMLParseState::NotEndTag;
			}
			break;
		case XMLParseState::NotEndTag:
			switch (thisChar)
			{
			case '/':
				//Next char will be ">", which shouldn't be a problem for the parser
				state.node->endIndex = i;
				if (state.node->endIndex < state.node->startIndex)
				{
					throw std::invalid_argument("");
				}
				nodes.push_back(state.node);

				if (stackPos > 0)
				{
					stack[stackPos - 1].node->children.push_back(state.node);
				}
				state.state = XMLParseState::Start;
				break;
			case '>':
				//this is a start tag
				state.state = XMLParseState::Start;
				stack.push_back(state);
				stackPos++;
				state = xmlParsingStackMember(Start, new xmlNode(buffer));
				break;
			default:
				checkForQuotes(thisChar, &stackPos, &stack, &state);
				break;
			}
			break;
		case XMLParseState::EndTag:
			if (thisChar == '>')
			{
				stackPos--;
				state = stack[stackPos];
				stack.pop_back();
				state.node->endIndex = i;
				if (state.node->endIndex < state.node->startIndex)
				{
					throw std::invalid_argument("");
				}
				if (stackPos > 0)
				{
					stack[stackPos - 1].node->children.push_back(state.node);
				}
				nodes.push_back(state.node);
			}
			else
			{
				checkForQuotes(thisChar, &stackPos, &stack, &state);
			}
			break;
		case XMLParseState::DQuote:
			if (thisChar == '"')
			{
				stackPos--;
				state = stack[stackPos];
				stack.pop_back();
			}
			break;
		case XMLParseState::SQuote:
			if (thisChar == '\'')
			{
				stackPos--;
				state = stack[stackPos];
				stack.pop_back();
			}
			break;
		}
	}

	return nodes;
}

xmlNodeVector daeParser::mapXmlNodes(const xmlNodeVector& input, std::function<xmlNode(const xmlNode&)> toMap)
{
	//TODO: make parallel
	xmlNodeVector toReturn;
	std::transform(input.begin(), input.end(), std::back_inserter(toReturn),
	               [&](const xmlNode& node) -> xmlNode { return toMap(node); });
	return toReturn;
}

void daeParser::alterXmlNodes(xmlNodeStore& input, std::function<void(xmlNode*)> toMap)
{
	//TODO: make parallel
	for (auto& i : input)
	{
		toMap(i);
	}
}

xmlNodeStore daeParser::parseIndexBuffer(const std::vector<char>& buffer, xmlNodeStore indexBuffer)
{
	alterXmlNodes(indexBuffer, [&](xmlNode* node) -> void
	{
		int index = node->startIndex;
		while (buffer[index] != '>')
		{
			index++;
		}
		std::vector<int> indexes;
		do
		{
			index++;
			int ind = parseAnInt(&index, buffer);
			indexes.push_back(ind);
		}
		while (buffer[index] == ' ');
		node->indexesIfApplicable = indexes;
	});
	return indexBuffer;
}

std::string daeParser::removeLeadingHash(const std::string& toRemove)
{
	return toRemove.substr(1, toRemove.size() - 1);
}

paramInfo::paramInfo(int stride, int idx)
{
	this->stride = stride;
	this->idx = idx;
}
