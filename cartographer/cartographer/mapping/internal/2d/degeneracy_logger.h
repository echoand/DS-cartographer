#ifndef CARTOGRAPHER_MAPPING_INTERNAL_2D_DEGENERACY_LOGGER_H_
#define CARTOGRAPHER_MAPPING_INTERNAL_2D_DEGENERACY_LOGGER_H_

#include <fstream>
#include <iomanip>
#include <Eigen/Dense>
#include <string>

namespace cartographer {
namespace mapping {

struct DegeneracyMetrics {
    double timestamp;
    double l1, l2, A, I, eta;
        // 新增（baseline）
    double ratio;
    double deg_eigen;   // 用 double（0/1），方便CSV
};

class InlineDegeneracyLogger {
public:
    // 构造函数传入路径，灵活性更高
    explicit InlineDegeneracyLogger(const std::string& filepath) {
        file_.open(filepath, std::ios::out);
        if (file_.is_open()) {
            file_ << "timestamp,l1,l2,A,I,eta,ratio,deg_eigen" << std::endl;
        }
    }

    ~InlineDegeneracyLogger() {
        if (file_.is_open()) file_.close();
    }

    void Log(const DegeneracyMetrics& data) {
        if (!file_.is_open()) return;
        file_ << std::fixed << std::setprecision(6)
          << data.timestamp << ","
          << data.l1 << ","
          << data.l2 << ","
          << data.A << ","
          << data.I << ","
          << data.eta << ","
          << data.ratio << ","
          << data.deg_eigen
          << std::endl;
    }

private:
    std::ofstream file_;
};

}  // namespace mapping
}  // namespace cartographer

#endif  // CARTOGRAPHER_MAPPING_INTERNAL_2D_DEGENERACY_LOGGER_H_