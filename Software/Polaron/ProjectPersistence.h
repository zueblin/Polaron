#ifndef ProjectPersistence_h
#define ProjectPersistence_h

class Sequencer;
class ProjectPersistence {
   public:
    ProjectPersistence(){};
    void init();
    void save(const char *filename, Sequencer * sequencer);
    void load(const char *filename, Sequencer * sequencer);
   private:
    boolean sdCardInitialized = false;
};

#endif
