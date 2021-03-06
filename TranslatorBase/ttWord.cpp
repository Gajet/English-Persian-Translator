#include "ttWord.h"

using namespace std;
using namespace TranslationTools;

ttImplementRTTI(ttWord);

ttWord::ttWord(ttWord::wordType pWord): mWord(pWord)
{

}

ttWord::ttWord(const ttWord& input) : ttObject(input), mWord(input.mWord)
{
}

void ttWord::print(ostream& stream)
{
	stream << mWord;
}

void ttWord::save(ttFileOManager& buffer,void*target)const
{
	buffer.saveParent<ttObject>();
	memcpy(target,mWord.c_str(),mWord.size() * 2);
}
void ttWord::load(ttFileIManager& buffer,void* data,int size)
{
	buffer.loadParent<ttObject>();
	mWord.assign((wchar_t*)data,size / 2);
}

int ttWord::size()const
{
	return mWord.size() * 2;
}

ttObject* ttWord::clone() const
{
	ttWord* result = new ttWord;
	new (result)ttWord(*this);
	return result;
}

bool ttWord::operator== (const ttObject& pRight)const
{
	if (ttObject::operator==(pRight) == false)
		return false;
	if (dynamic_cast<const ttWord*>(&pRight) == NULL || dynamic_cast<const ttWord*>(&pRight)->mWord != mWord)
		return false;
	return true;
}

ostream& operator <<(ostream& stream,ttWord::wordType w)
{
	char* target = (char*)w.c_str();
	for (unsigned i=0;i<w.length();i++,target += 2)
		stream .put(target[0]).put(target[1]);
	return stream;
}
