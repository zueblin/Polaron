#ifndef ProjectPersistence_h
#define ProjectPersistence_h

class Sequencer;
class ProjectPersistence {
   public:
    ProjectPersistence(){};
    void init();
    void save(int projectNum, Sequencer * sequencer);
    void load(int projectNum, Sequencer * sequencer);
    boolean exists(int projectNum);
    boolean isActive(int projectNum);
   private:
    void updateProjectList();
    boolean sdCardInitialized = false;
    uint16_t existingProjects = 0;
    uint16_t activeProject = 0;

};

#endif
