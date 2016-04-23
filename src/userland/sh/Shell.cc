#include "Shell.h"

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#if defined (__cplusplus)
}
#endif

void Shell::loop(void)
{
	char cmd_line[CMD_LINE_MAX_SIZE + 1];
	command_t cmd;
	int status = 0;
	
	while (0 == status)
	{
		printPrompt();
		readCommand(cmd_line);
		cmd = parseCommand(cmd_line);
		status = executeCommand(cmd);	
	}
}

void Shell::printPrompt(void)
{
	printf("\n");
	printf("TODO");
	printf("> ");
	fflush(stdout);
	cursor_base_position_ = 6;
	cursor_current_position_ = cursor_base_position_;
}

void Shell::readCommand(char* line)
{
	line[0] = '\0';
	char c;
	while (1)
	{
		c = getchar();
		switch (c)
		{
			case 0x08: /* backspace */
			{
				if (cursor_current_position_ <= cursor_base_position_)
				{
					continue;
				}
				else
				{
					printf("%c", c);
					cursor_current_position_--;
					line[cursor_current_position_ - cursor_base_position_] = '\0';
				}
				break;
			}
			case 0x09: /* horizontal tab */
			{
				continue;
				break;
			}
			case '\n':
			{
				line[cursor_current_position_ - cursor_base_position_] = '\0';
				break;
			}
			default:
			{
				if (c >= 0x20 && c < 0xff && (cursor_current_position_ - cursor_base_position_ < CMD_LINE_MAX_SIZE))
				{
					printf("%c", c);
					line[cursor_current_position_ - cursor_base_position_] = c;
					cursor_current_position_++;
					line[cursor_current_position_ - cursor_base_position_] = '\0';
				}
				break;
			}
		}
		if ('\n' == c) break;
	}
	return;
}

command_t Shell::parseCommand(char* cmd_line)
{
	command_t cmd;
	return cmd;
}

int Shell::executeCommand(command_t cmd)
{
	return 0;
}
