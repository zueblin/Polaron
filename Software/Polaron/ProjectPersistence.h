#ifndef ProjectPersistence_h
#define ProjectPersistence_h

class Sequencer;
class ProjectPersistence {
   public:
    ProjectPersistence(){};
    void init();
    void save(const char *filename, Sequencer * sequencer);
    void load(const char *filename, Sequencer * sequencer);
    boolean exists(const char *filename);
   private:
    boolean sdCardInitialized = false;
};

#endif
