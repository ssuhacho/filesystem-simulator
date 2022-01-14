/**
* [Suha Cho]
* [FilesystemTree.cpp]
**/

#include "FilesystemTree.h"
#include <fstream>
#include <stdexcept>


bool FilesystemTree::create(const std::string& pName, int pType,
	const std::string& parentPath)
{
	bool rValue = false;

	//make a node named pName of type pType.
	std::shared_ptr<FSNode> newNodePtr = std::make_shared<FSNode>(pName, pType);
	//make another node at the path parentPath.
	std::shared_ptr<FSNode> parentPtr = pathToPointer(parentPath);

	if (pName != ROOT_NAME && pName != parentPtr->getName())
	{
		//add a child node to the parent node.
		parentPtr->addChild(newNodePtr);

		//returns true if the node was successfully added.
		rValue = true;
	}
	
	return rValue;
}

bool FilesystemTree::remove(const std::string& pName, const std::string& parentPath)
{
	bool rValue = false;

	//make a node at the path parentPath.
	std::shared_ptr<FSNode> parentPtr = pathToPointer(parentPath);

	//remove the node named pName from the directory at the path parentPath. 
	if (pName != ROOT_NAME && parentPtr != nullptr && parentPtr->removeChild(pName))
	{
		//Returns true if the node was successfully removed.
		rValue = true;
	}

	return rValue;
}


bool FilesystemTree::move(const std::string& pName, const std::string& sourcePath,
	const std::string& destPath)
{
	bool rValue = false;
	
	//make a node at the path sourcePath.
	std::shared_ptr<FSNode> sourceParentPtr = pathToPointer(sourcePath);
	//make a node at the path destPath.
	std::shared_ptr<FSNode> destParentPtr = pathToPointer(destPath);

	//should not allow the root node to be moved.
	//if pName is a name of a directory, it moves the directory.
	if (pName != ROOT_NAME 
		&& sourceParentPtr != nullptr 
		&& destParentPtr != nullptr 
		&& nullptr != sourceParentPtr->getChild(pName))
	{
		//move the node named pName from the directory at the path sourcePath, 
		std::shared_ptr<FSNode> moveNode = sourceParentPtr->getChild(pName);

		//to the directory at destPath.
		if (destParentPtr->addChild(moveNode))
		{
			//remove the child from the directory at the path sourcePath.
			sourceParentPtr->removeChild(pName);

			//Returns true if the move was successful.
			rValue = true;
		}
	}

	return rValue;
}

std::shared_ptr<FSNode> FilesystemTree::copySubTree(std::shared_ptr<FSNode> rootPtr) const
{
	std::shared_ptr<FSNode> rPtr = nullptr;

	if (rootPtr != nullptr)
	{
		//copy node
		rPtr = std::make_shared<FSNode>(rootPtr->name, rootPtr->type);

		//copy tree nodes during a traversal
		for (int i = 0; i < rootPtr->getNumChildren(); i++) 
		{
			//recursive copy
			rPtr->addChild(copySubTree(rootPtr->getChild(i)));
		}
	}

	//Returns a fully independent copy of the subtree which begins at rootPtr. 
	return rPtr;

}

bool FilesystemTree::copy(const std::string& pName, const std::string& sourcePath,
	const std::string& destPath)
{
	bool rValue = false;

	//make a node at the path sourcePath.
	std::shared_ptr<FSNode> sourceParentPtr = pathToPointer(sourcePath);
	//make a node at the path destPath.
	std::shared_ptr<FSNode> destParentPtr = pathToPointer(destPath);

	//should not allow the root node to be copied.
	//if pName is a name of a directory it copies the directory.
	if (pName != ROOT_NAME 
		&& sourceParentPtr != nullptr 
		&& destParentPtr != nullptr 
		&& nullptr != sourceParentPtr->getChild(pName)) 
	{
		//copy the node named pName from the directory at the path sourcePath,
		std::shared_ptr<FSNode> copyNode = copySubTree(sourceParentPtr->getChild(pName));

		//to the directory at destPath. 
		if (destParentPtr->addChild(copyNode)) 
		{
			//Returns true if the copy was successful.
			rValue = true;
		}
	}

	return rValue;
}




/***********************************************************************/
/************* YOU SHOULD NOT NEED TO EDIT BELOW THIS LINE *************/
/***********************************************************************/

int FilesystemTree::find(const std::string& pName, const std::string& startPath,
	std::ostream& outStream) const
{
	int matches = 0;  // no results found
	std::shared_ptr<FSNode> nodePtr = pathToPointer(startPath);

	if (nodePtr != nullptr)
	{
		for (int i = 0; i < nodePtr->getNumChildren(); i++)
		{
			if (pName == nodePtr->getChild(i)->getName())
			{
				outStream << startPath + SEPARATING_CHAR + pName << std::endl;
				matches++;
			}
		}

		// recurse subdirectories
		for (int i = 0; i < nodePtr->getNumChildren(); i++)
		{
			if (nodePtr->getChild(i)->isDirectory())
			{
				matches += find(pName,
					startPath + SEPARATING_CHAR + nodePtr->getChild(i)->getName(),
					outStream);
			}
		}
	}

	return matches;

}

bool FilesystemTree::format()
{
	bool rValue = true;

	// Set all children pointers to null. Should erase all subtrees
	// since these are smart pointers.
	while (rootPtr->getNumChildren() > 0)
	{
		rValue = (rValue && rootPtr->removeChild(rootPtr->getChild(0)->getName()));
	}

	return rValue;
}

FilesystemTree& FilesystemTree::operator=(const FilesystemTree& rTree)
{
	// What if there's an existing tree? Once rootPtr gets assigned a new value
	// the old root node gets auto-removed because smart pointers.
	rootPtr = copySubTree(rTree.rootPtr);

	return *this;
}

FilesystemTree::FilesystemTree(const FilesystemTree& treeToCopy)
{
	*this = treeToCopy;  // use overloaded =
}

std::pair<int, int> FilesystemTree::recursiveStats(std::shared_ptr<FSNode> startPtr) const
{
	std::pair<int, int> count = { 0,0 };

	if (startPtr != nullptr)
	{
		// count myself unless I'm root
		if (startPtr->getName() != ROOT_NAME)
		{
			count.first += startPtr->isDirectory();
		}
		count.second += startPtr->isFile();

		// recurse on children if this is a directory
		if (startPtr->isDirectory())
		{
			std::pair<int, int> rCount = { 0,0 };

			for (int i = 0; i < startPtr->getNumChildren(); i++)
			{
				rCount = recursiveStats(startPtr->getChild(i));
				count.first += rCount.first;
				count.second += rCount.second;
			}
		}
	}

	return count;

}

void FilesystemTree::displayStats(std::ostream& outStream) const
{
	std::pair<int, int> count = recursiveStats(rootPtr);

	outStream << "Directories: " << count.first << std::endl;
	outStream << "Files: " << count.second << std::endl;

}

FilesystemTree::FilesystemTree()
{
	rootPtr = std::make_shared<FSNode>("", 1);
	rootPtr->name = ROOT_NAME; // get around alpha/digit name restriction
}

FilesystemTree::FilesystemTree(const std::string& fileName)
{
	std::ifstream inFile;
	std::string command;
	std::string name;
	std::string path;
	int type;

	rootPtr = std::make_shared<FSNode>("", 1);
	rootPtr->name = ROOT_NAME; // get around alpha/digit name restriction

	inFile.open(fileName.c_str());
	if (inFile.fail())
	{
		throw std::runtime_error("File " + fileName + " not found.");
	}
	else
	{
		while (std::getline(inFile, command))
		{
			type = -1;
			name = "";
			path = "";

			if (command[0] == '*') // create directory
			{
				command = command.substr(2, command.length() - 2);
				type = 1;
			}
			else
			{
				type = 0;
			}

			name = command.substr(0, command.find(' '));
			path = command.substr(command.find(' ') + 1,
				command.length() - command.find(' ') + 1);

			if (type != -1 && name != "" && path != "")  // verify valid command
			{
				if (!create(name, type, path))
				{
					std::cout << "ERROR: Could not create " << name
						<< " in path " << path << std::endl << std::endl;
				}
			}

		}
	}
}

std::shared_ptr<FSNode> FilesystemTree::pathToPointer(const std::string& path) const
{
	std::string pathCpy = path;

	int startIdx = 0;
	int slashIdx = 0;
	std::shared_ptr<FSNode> parentPtr = rootPtr;

	if (pathCpy == ROOT_NAME)
	{
		return rootPtr;
	}

	if (pathCpy == "" || (pathCpy.substr(0, ROOT_NAME.length()) != ROOT_NAME))
	{
		return nullptr; // path doesn't start with ROOT_NAME
	}

	// Add a last / if there isn't one
	if (pathCpy[pathCpy.length() - 1] != SEPARATING_CHAR)
	{
		pathCpy += SEPARATING_CHAR;
	}

	slashIdx = pathCpy.find(SEPARATING_CHAR);
	pathCpy = pathCpy.substr(slashIdx + 1, pathCpy.length() - slashIdx + 1);

	while (parentPtr != nullptr && pathCpy.length() > 1)
	{
		slashIdx = pathCpy.find(SEPARATING_CHAR);
		parentPtr = parentPtr->getChild(pathCpy.substr(0, slashIdx));
		pathCpy = pathCpy.substr(slashIdx + 1, pathCpy.length() - slashIdx + 1);
	}

	return parentPtr;
}

void FilesystemTree::displayTree(std::ostream& outStream) const
{
	recursiveDisplay(rootPtr, "", outStream);
}

void FilesystemTree::recursiveDisplay(std::shared_ptr<FSNode> nodePtr,
	std::string preSpace, std::ostream& outStream) const
{
	outStream << preSpace << (nodePtr->isDirectory() ? "<" : "")
		<< nodePtr->getName() << (nodePtr->isDirectory() ? ">" : "")
		<< std::endl;

	if (nodePtr->isDirectory())
	{

		for (int i = 0; i < nodePtr->getNumChildren(); i++)
		{
			recursiveDisplay(nodePtr->getChild(i), preSpace + "|     ", outStream);
		}
	}
}

FilesystemTree::~FilesystemTree()
{
	rootPtr = nullptr; // Strictly speaking not necessary because smart pointers.
}