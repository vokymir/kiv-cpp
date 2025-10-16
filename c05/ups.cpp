#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// to class rika, ze je treba rikat OpCode
enum class OpCode {
  Login,
  Join_Room,
  Leave_Room,
  Turn,
};

class Packet {
public:
  Packet(OpCode opcode) { mOpcode = opcode; }
  // serialize to
  std::string Build() {
    std::ostringstream out;

    out << "CPP!";
    out << std::setw(4) << std::setfill('0') << static_cast<int>(mOpcode);
    Serialize(out);

    return out.str();
  }

protected:
  virtual void Serialize(std::ostream &out) = 0;

private:
  OpCode mOpcode;
};

class LoginPacket : public Packet {
public:
  LoginPacket() : Packet(OpCode::Login) {}
  void Set_Credentials(const std::string &username) { mUsername = username; }

protected:
  void Serialize(std::ostream &out) override {
    out << mUsername.length() << ";" << mUsername;
  }

private:
  std::string mUsername;
};

int main() {
  LoginPacket p;
  p.Set_Credentials("javok");

  std::cout << p.Build() << std::endl;
  return 0;
}
