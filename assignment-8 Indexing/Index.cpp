
#include "Index.hpp"

namespace ECE141 {

  StatusResult Index::writeToStorage(Storage& aStorage, uint32_t startBlockNum) {
  	int curPos = 0;
  	uint32_t blockNum = startBlockNum;
  	Block theBlock{kIndexBlockType};
  	BufferWriter buffW = BufferWriter(theBlock.data, kPayloadSize);
  	for (auto thePair : index) {
  	  if (8 > kPayloadSize - buffW.tellp()) { //check this
  	  	curPos++;
  	  	if (curPos < blockNums.size()) { //we can still reuse some of our blocks
  	  	  theBlock.header.next = blockNums[curPos];
  	  	}
  	  	else { //need to write new blocks
  	  	  aStorage.writeBlock(blockNum, theBlock); //NOT GREAT, but at least gets it working
  	  	  //for now going to have to write twice in order to get next blocknum
  	  	  //if we have time... need to add another method to make this more efficient or change logic
  	  	  StatusResult theResult = aStorage.findFreeBlockNum();
  	  	  addBlockNum(theResult.value); //may also have to readjust iter...
  	  	  theBlock.header.next = theResult.value;
  	  	}
  	  	theBlock.header.size = kPayloadSize/8; //NEED TO FIX THIS... VERY BAD HACK
  	  	aStorage.writeBlock(blockNum, theBlock);
  	  	blockNum = theBlock.header.next;
  	  	buffW.close();
  	  }
  	  buffW << (uint32_t)const_cast<Value&>(thePair.first) << thePair.second; //assuming only dealing w/ primary key
  	}
  	uint32_t finalBlockSize = (index.size())%126;
  	theBlock.header.size = finalBlockSize;
  	theBlock.header.next = 0;
  	aStorage.writeBlock(blockNum, theBlock);
  	curPos++;

  	//after this, still need to check if we have blockNums that we need to free
    while (curPos < blockNums.size()) {
  	  aStorage.releaseBlock(blockNums[curPos]);
  	  curPos++;
  	}
  	dirty = false;
  	return StatusResult{noError};
  }

  StatusResult Index::dropIndex(Storage &aStorage) {
  	for (auto blockNum:blockNums) {
  		aStorage.releaseBlock(blockNum);
  	}
  	return StatusResult{noError};
  }

  Index& Index::decodeFromBlock(Block& aBlock) {
  	Block theBlock = aBlock;
  	uint32_t theValue; //assuming only primary key ids are going to be used...
  	int numItems = aBlock.header.size;
  	uint32_t theBlockNum;
  	BufferReader buffR = BufferReader(theBlock.data, kPayloadSize, kPayloadSize);
  	for (int i=0; i < numItems; i++) {
  	  buffR >> theValue;
  	  Value aValue = (int)theValue; //might need to convert this to int
  	  buffR >> theBlockNum;
  	  index.insert({aValue, theBlockNum});
  	}
  	//so instead of trying to have a constructor make an index from
  	//multiple blocks... let's just generate a default index... and
  	//then update this method later

  	//need to remember to add each block to blockNum list

  	//so when actually encoding block, we just need to encode value & uint32_t
  	//which should be handled for us... we may also want to include initial info as
  	//to what type of data we should expect this to be... may also have to keep reference
  	//to Entity object
  	return *this;
  }

  Index& Index::addKeyValue(const Value &aKey, uint32_t aValue) {
  	if (index.count(aKey) == 0) {
  	  index.insert({const_cast<Value&>(aKey), aValue});
  	  dirty = true;
  	}
    return *this;
  }  
    
  Index& Index::removeKeyValue(const Value &aKey) {
  	if (index.count(aKey)) {
  	  index.erase(aKey);
  	  dirty = true;
  	}
  	return *this;
  }

  Index& Index::addBlockNum(uint32_t theBlockNum) {
  	blockNums.push_back(theBlockNum);
  	return *this;
  }

  /*Index& Index::markDirty(bool aState) {
  	dirty = aState;
  	return *this;
  }*/

  bool Index::isDirty() {
  	return dirty;
  }
    
  bool Index::contains(const Value &aKey) {
  	if (index.count(aKey)) {
  	  return true;
  	}
  	return false;
  }
    
  uint32_t Index::getValue(const Value &aKey) {
  	if (index.count(aKey)) {
  	  return index[aKey];
  	}
  	return 0;
  }

  StatusResult Index::each(Storage& aStorage, StorageCallback aCallback) {
    Block         theBlock;
    StatusResult  theResult{noError};
    
    for(auto thePair : index) {
      theResult=aStorage.readBlock(thePair.second, theBlock);
      if(theResult) {
        theResult = aCallback(aStorage, theBlock, thePair.second);
        if(!theResult) break;
      }
      else break;
    }
    return theResult;
  }
}
