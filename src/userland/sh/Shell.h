#ifndef _SHELL_H_
#define _SHELL_H_

#define CMD_LINE_MAX_SIZE 255
#define MAX_ARGS 64

struct command_t
{
	char* name;
	int argc;
	char* argv[MAX_ARGS];
};

class Shell
{
public:
	void loop(void);
	void printPrompt(void);
	void readCommand(char* cmd);
	command_t parseCommand(char* cmd_line);
	int executeCommand(command_t cmd);

private:
	unsigned int cursor_current_position_;
	unsigned int cursor_base_position_;
};

#endif /* _SHELL_H_ */
