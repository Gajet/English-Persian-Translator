﻿#include "PersianParser.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace TranslationTools;

PersianParser::PersianParser(void)
{
	ttFileIManager inputfile;
	mDictionary = dynamic_cast<ttDictionary*>(inputfile.initializeLoad("PersianDictionary.srz"));
	if (mDictionary == NULL)
		mDictionary = new ttDictionary;
	mDictionary->retain();
}

TranslationTools::ttArray* PersianParser::parse(TranslationTools::ttWord::wordType script)
{
	vector<ttWord::wordType> paragraphs;
	explode(script,L"\n",paragraphs);
	ttArray* root = new ttArray;
	for(unsigned i=0;i<paragraphs.size();i++)
	{
		vector<ttWord::wordType> senteces;
		explode(paragraphs[i],L".;!?,",senteces);
		ttArray* paragraph = new ttArray;
		//wcout << L"Paragraph " << i << " : \n";
		for(unsigned j=0;j<senteces.size();j++)
		{
			vector<ttWord::wordType> words;
			explode (senteces[j],L" ", words);
			int shift = 0;
			for(unsigned k=0;k + shift<words.size();k++)
			{
				words[k] = words[k+shift];
				if (words[k] == L"می")
				{
					words[k] += words[k+shift+1];
					shift ++;
				}
			}
			words.resize(words.size() - shift);
			ttArray& sentence = *new ttArray;
			for(unsigned i=0;i<words.size();i++)
			{
				sentence(i) = & mDictionary->findNearestWords(words[i]);
				if (static_cast<ttArray*>(sentence[i])->objNum() == 0)
				{
					ttWord::wordType tempWord = words[i],tempWord2;
					do
					{
						if (tempWord.find(L"یم",tempWord.length() - 2) != ttWord::wordType::npos)
						{
							tempWord.resize(tempWord.size() - 2);
							tempWord2 = L"ما";
							break;
						}
						if (tempWord.find(L"ید",tempWord.length() - 1) != ttWord::wordType::npos)
						{
							tempWord.resize(tempWord.size() - 2);
							tempWord2 = L"شما";
							break;
						}
						if (tempWord.find(L"ند",tempWord.length() - 1) != ttWord::wordType::npos)
						{
							tempWord.resize(tempWord.size() - 2);
							tempWord2 = L"آنها";
							break;
						}
						if (tempWord.find(L"م",tempWord.length() - 1) != ttWord::wordType::npos)
						{
							tempWord.resize(tempWord.size() - 1);
							tempWord2 = L"من";
							break;
						}
						if (tempWord.find(L"ی",tempWord.length() - 1) != ttWord::wordType::npos)
						{
							tempWord.resize(tempWord.size() - 1);
							tempWord2 = L"تو";
							break;
						}
					}
					while(false);
					sentence[i] = & mDictionary->findNearestWords(tempWord);
					if (static_cast<ttArray*>(sentence[i])->objNum() != 0)
					{
						words[i] = tempWord;
						words.insert(words.begin() + i, tempWord2);
					}
				}
			}
			/*for(unsigned i=0;i<words.size();i++)
			{
				if (dynamic_cast<ttArray*>(sentence[i])->objnum() == 0))
					sentence[i] = new ttWord(words[i]);
				if (dynamic_cast<ttArray*>(sentence[i])->objnum() == 1))
					sentence[i] = dynamic_cast<ttArray*>(sentence[i])->operator[](0);
			}*/

			pair<ttWordType*,ttPhraseRole*> needed [3];
			needed[0].first = new ttWordType(ttWordType::wt_Noun);
			needed[1].first = new ttWordType(ttWordType::wt_Noun);
			needed[2].first = new ttWordType(ttWordType::wt_Verb);
			needed[0].second = new ttPhraseRole(ttPhraseRole::pr_Subject);
			needed[1].second = new ttPhraseRole(ttPhraseRole::pr_Object);
			needed[2].second = new ttPhraseRole(ttPhraseRole::pr_Verb);
			int neededFound = 0;
			ttArray* result = new ttArray(new ttArray,new ttArray,new ttArray,NULL);
			for(unsigned i=0;i<words.size() && neededFound < 3;i++)
			{
				if (dynamic_cast<ttArray*>(sentence[i]))
				{
					unsigned j;
					for(j=0;j<static_cast<ttArray*>(sentence[i])->objNum();j++)
						if (static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>())
							if ((*static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>()) == *needed[neededFound].first)
							{
								static_cast<ttArray*>(result->operator ()(neededFound))->operator()(static_cast<ttArray*>(result->operator ()(neededFound))->objNum()) = 
									new ttWordID(mDictionary->findWord(*static_cast<ttWord*>(static_cast<ttArray*>(sentence[i])->operator[](j))));
								//result->operator ()(neededFound) = new ttArray(new ttWordID(mDictionary->findWord(
								//	*static_cast<ttWord*>(static_cast<ttArray*>(sentence[i])->operator[](j)))));
								result->operator ()(neededFound)->attachMetaData(needed[neededFound].second);
								neededFound ++;
								break;
							}
					if (j < static_cast<ttArray*>(sentence[i])->objNum())
						continue;
					if (neededFound == 1 || neededFound == 2)
					for(j=0;j<static_cast<ttArray*>(sentence[i])->objNum();j++)
						if (static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>())
							if (*static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>() == ttWordType::wt_Adjective)
							{
								static_cast<ttArray*>(result->operator ()(neededFound - 1))->operator()(static_cast<ttArray*>(result->operator ()(neededFound - 1))->objNum()) = 
									new ttWordID(mDictionary->findWord(*static_cast<ttWord*>(static_cast<ttArray*>(sentence[i])->operator[](j))));
							}
					if (neededFound == 1 || neededFound == 2)
					for(j=0;j<static_cast<ttArray*>(sentence[i])->objNum();j++)
						if (static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>())
							if (*static_cast<ttArray*>(sentence[i])->operator[](j)->getMetaData<ttWordType>() == ttWordType::wt_Adverb)
							{
								static_cast<ttArray*>(result->operator ()(2))->operator()(static_cast<ttArray*>(result->operator ()(2))->objNum()) = 
									new ttWordID(mDictionary->findWord(*static_cast<ttWord*>(static_cast<ttArray*>(sentence[i])->operator[](j))));
							}
				}
				
			}
			paragraph->operator()(paragraph->objNum()) = result;
		}
		root ->operator()(root->objNum()) = paragraph;
	}
	return root;
}

TranslationTools::ttWord::wordType PersianParser::translate(TranslationTools::ttArray* pRoot)
{
	wstring result;
	ttPhraseRole* needed[3] = {new ttPhraseRole(ttPhraseRole::pr_Subject), new ttPhraseRole(ttPhraseRole::pr_Object), new ttPhraseRole(ttPhraseRole::pr_Verb)};
	for(unsigned i=0;i<pRoot->objNum();i++)
	{
		if(ttArray* paragraph = dynamic_cast<ttArray*>(pRoot->operator[](i)))
		{
			for(unsigned j = 0; j < paragraph->objNum();j++)
				if (ttArray* sentence = dynamic_cast<ttArray*>(paragraph->operator[](j)))
				{
					for(unsigned found = 0;found < 3;found ++)
						for(unsigned k=0;k<sentence->objNum();k++)
							if (sentence->operator[](k)->getMetaData<ttPhraseRole>() && sentence->operator[](k)->getMetaData<ttPhraseRole>()->operator== (*needed[found]))
							{
								if (ttArray* words = dynamic_cast<ttArray*>(sentence->operator[](k)))
								{
									for(unsigned l = 0;l < words->objNum();l ++)
									{
										words->operator[](l) = mDictionary->getWord(static_cast<ttWordID*>(words->operator[](l))->getID());
										if (words->operator[](l)->getMetaData<ttWordType>()->operator == (ttWordType::wt_Noun) ||
											words->operator[](l)->getMetaData<ttWordType>()->operator == (ttWordType::wt_Verb))
											result += static_cast<ttWord*>(words->operator[](l))->getWord() + L" ";
									}
									for(unsigned l = 0;l < words->objNum();l ++)
										if (!words->operator[](l)->getMetaData<ttWordType>()->operator == (ttWordType::wt_Noun) &&
											!words->operator[](l)->getMetaData<ttWordType>()->operator == (ttWordType::wt_Verb))
											result += static_cast<ttWord*>(words->operator[](l))->getWord() + L" ";
								}
							}
					result += L'.';
				}
			result += L'\n';
		}
	}
	return result;
}

PersianParser::~PersianParser(void)
{
	if (mDictionary == NULL)
		return;
	ttFileOManager outputfile;
	outputfile.initializeSave(mDictionary);
	outputfile.flush("PersianDictionary.srz");
	mDictionary->release();
}
