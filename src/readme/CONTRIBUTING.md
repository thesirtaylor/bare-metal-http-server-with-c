# Contributing to HTTP Server in C

Thank you for your interest in contributing! This project is primarily educational, so contributions that improve clarity, fix bugs, or enhance documentation are especially valuable.

## 🎯 Project Goals

Remember, this project aims to:
- Teach fundamental web server concepts
- Be readable and understandable
- Use minimal dependencies
- Show how things work under the hood

## 📋 How to Contribute

### Reporting Bugs

1. Check if the issue already exists
2. Include steps to reproduce
3. Include your OS and compiler version
4. Include relevant error messages or logs

### Suggesting Enhancements

1. Explain the use case
2. Describe how it helps with learning
3. Keep it aligned with project goals (education over optimization)

### Pull Requests

1. **Fork the repository**
2. **Create a branch** (`git checkout -b feature/your-feature`)
3. **Make your changes**
   - Keep code simple and readable
   - Add comments explaining complex parts
   - Update documentation if needed
4. **Test thoroughly**
   ```bash
   make clean
   make
   ./bin/webserver
   # Test all endpoints
   ```
5. **Commit your changes** with clear messages
6. **Push to your fork**
7. **Open a Pull Request**

## 📝 Code Style

### C Code Guidelines

```c
// Good: Clear variable names
int client_socket = accept(server_socket, ...);

// Bad: Unclear names
int s = accept(s2, ...);

// Good: Comments explaining WHY
// Accept blocks until a client connects
int client_fd = accept(server_fd, ...);

// Bad: Comments explaining WHAT (obvious from code)
// Call accept
int client_fd = accept(server_fd, ...);
```

### Formatting
- Use 4 spaces for indentation (no tabs)
- Opening braces on same line for functions
- Clear spacing between logical blocks
- Max line length: ~100 characters

### Comments
- Explain WHY, not WHAT
- Add context for non-obvious code
- Document any networking or system concepts
- Include examples where helpful
 
```

## 📚 Documentation

When adding features:
1. Update README.md with new endpoints
2. Add to relevant .md files in docs
3. Update code comments
4. Add curl examples

## ✅ Checklist for Pull Requests

- [ ] Code builds without warnings
- [ ] All endpoints tested manually
- [ ] Code is well-commented
- [ ] Documentation updated
- [ ] Follows existing code style
- [ ] No external dependencies added (unless discussed)
- [ ] Changes are educational and clear

## 🎓 Educational Focus

This project prioritizes:
1. **Clarity** over performance
2. **Understanding** over features
3. **Simplicity** over abstraction
4. **Learning** over production readiness

If a change makes the code harder to understand, it probably doesn't fit.

## 💡 Ideas for Contributions

### Beginner-Friendly
- Fix typos in documentation
- Improve code comments
- Add more examples to docs
- Test on different platforms

### Intermediate
- Add new educational endpoints
- Improve error messages
- Add more test cases
- Enhance documentation

### Advanced
- Allow to handle multiple clients concurrently
- Add SSL/TLS support
- Implement HTTP/2
- Add WebSocket support
- Improve JSON parser

## 📬 Questions?

Open an issue with the `question` label!

## 🙏 Thank You

Every contribution helps make this a better learning resource. Thank you for being part of this educational project!