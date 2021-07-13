#include <iostream>
#include <map>
#include <mutex>
#include <thread>

std::map<int, double> processRemainingTimes;
std::mutex processRemainingTimesMutex;

void setProcessRemainingTime(int pid, double time) {
    std::lock_guard<std::mutex> guard(processRemainingTimesMutex);
    processRemainingTimes[pid] = time;
}

double getProcessRemaningTime(int pid) {
    std::lock_guard<std::mutex> guard(processRemainingTimesMutex);
    if (processRemainingTimes.find(pid) != processRemainingTimes.end()) {
        return processRemainingTimes[pid];
    } else {
        std::cerr << "Process ID not found." << std::endl;
        return -1;
    }
}

class List {
public:
    bool addToList(int element) {
        std::lock_guard<std::mutex> guard(listMutex);
        auto result = processes.insert(element);
        return result.second;
    }

    bool removeFromList(int element) {
        std::lock_guard<std::mutex> guard(listMutex);
        return processes.erase(element) > 0;
    }

private:
    std::set<int> processes;
    std::mutex listMutex;

    friend class Iterator;
};

class Iterator {
public:
    Iterator(List list) : processList(list), iter(processList.processes.begin()) {}

    int getNext() {
        std::lock_guard<std::mutex> guard(iteratorMutex);
        if (iter == processList.processes.end()) {
            return -1;
        } else {
            return *(iter++);
        }
    }

private:
    List processList;
    std::set<int>::iterator iter;
    std::mutex iteratorMutex;
};

int getNextProcessToRun(int leavingProcessID, List list) {
    list.removeFromList(leavingProcessID);
    Iterator i(list);
    int processid = i.getNext();
    int processbrusttime = getProcessRemaningTime(processid);
    int minbrusttime = processbrusttime;
    int minbrusttimeprocessid = processid;
    while (processid != -1) {
        processbrusttime = getProcessRemaningTime(processid);
        if (minbrusttime > processbrusttime) {
            minbrusttime = processbrusttime;
            minbrusttimeprocessid = processid;
        }
        processid = i.getNext();
    }
    return minbrusttimeprocessid;
}

int main() {
    setProcessRemainingTime(1, 5.0);
    setProcessRemainingTime(2, 3.5);
    setProcessRemainingTime(3, 7.2);

    List processList;
    processList.addToList(1);
    processList.addToList(2);
    processList.addToList(3);

    int nextProcess = getNextProcessToRun(1, processList);
    std::cout << "Next process to run: " << nextProcess << std::endl;

    return 0;
}
