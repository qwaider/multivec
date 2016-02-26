#pragma once
#include "utils.hpp"

class MonolingualModel
{
    friend class BilingualModel;
    friend void save(ofstream& outfile, const MonolingualModel& model);
    friend void load(ifstream& infile, MonolingualModel& model);

private:
    mat input_weights;
    mat output_weights; // output weights for negative sampling
    mat output_weights_hs; // output weights for hierarchical softmax
    mat sent_weights;

    long long training_words; // total number of words in training file (used to compute word frequencies)
    long long training_lines;
    long long words_processed;

    float alpha;
    Config config;
    unordered_map<string, HuffmanNode> vocabulary;
    vector<HuffmanNode*> unigram_table;

    void addWordToVocab(const string& word);
    void reduceVocab();
    void createBinaryTree();
    void assignCodes(HuffmanNode* node, vector<int> code, vector<int> parents) const;
    void initUnigramTable();

    HuffmanNode* getRandomHuffmanNode(); // uses the unigram frequency table to sample a random node

    vector<HuffmanNode> getNodes(const string& sentence) const;
    void subsample(vector<HuffmanNode>& node) const;

    void readVocab(const string& training_file);
    void initNet();
    void initSentWeights();

    void trainChunk(const string& training_file, const vector<long long>& chunks, int chunk_id);

    int trainSentence(const string& sent, int sent_id);
    void trainWord(const vector<HuffmanNode>& nodes, int word_pos, int sent_id);
    void trainWordCBOW(const vector<HuffmanNode>& nodes, int word_pos, int sent_id);
    void trainWordSkipGram(const vector<HuffmanNode>& nodes, int word_pos, int sent_id);

    vec hierarchicalUpdate(const HuffmanNode& node, const vec& hidden, float alpha, bool update = true);
    vec negSamplingUpdate(const HuffmanNode& node, const vec& hidden, float alpha, bool update = true);

    vector<long long> chunkify(const string& filename, int n_chunks);
    vec wordVec(int index, int policy) const;
    
public:
    MonolingualModel() : training_words(0), training_lines(0), words_processed(0) {} // model with default configuration
    MonolingualModel(Config config) : training_words(0), training_lines(0), words_processed(0), config(config) {}

    vec wordVec(const string& word, int policy = 0) const; // word embedding
    vec wordVecOOV(const string& word, int policy = 0) const; // word embedding
    vec sentVec(const string& sentence, int policy = 0); // paragraph vector (Le & Mikolov)
    void sentVec(istream& infile, int policy); // compute paragraph vector for all lines in a stream

    void train(const string& training_file); // training from scratch (resets vocabulary and weights)

    void saveVectorsBin(const string &filename, int policy = 0) const; // saves word embeddings in the word2vec binary format
    void saveVectors(const string &filename, int policy = 0) const; // saves word embeddings in the word2vec text format
    void saveSentVectors(const string &filename) const;
    
    void load(const string& filename); // loads the entire model
    void save(const string& filename) const; // saves the entire model
    
    vec bow(const string& sequence) const;
    
    void normalizeWeights(); // normalize all weights between 0 and 1
    
    float similarity(const string& word1, const string& word2, int policy = 0) const;
    float distance(const string& word1, const string& word2, int policy = 0) const;
    float similarityNgrams(const string& seq1, const string& seq2, int policy = 0) const;
    float similaritySentence(const string& seq1, const string& seq2, int policy = 0) const;
};
