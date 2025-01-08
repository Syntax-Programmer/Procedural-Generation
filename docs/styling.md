# Code Style Guide

This guide aims to maintain a consistent coding style across the project to improve readability, maintainability, and efficiency.

---

## **1. Return Values for Success/Failure**

- **Success**: `0`
- **Failure**: `1`

Always use `0` for success and `1` for failure in function return values to maintain consistency and avoid ambiguity.

---

## **2. Boolean Representation**

- **Use `0`/`1`** instead of `true`/`false`.

In C, `0` represents `false` and any non-zero value represents `true`. For consistency and simplicity, avoid using `true` and `false`. Stick to `0` and `1` explicitly.

---

## **3. Custom Flags**

- **Type**: `Uint8`

For flags, always use `Uint8` (unsigned 8-bit integer). This minimizes memory usage and ensures efficient flag storage.

---

## **4. Memory Efficiency**

- **Compress**: Always strive to use the smallest possible memory size.

Minimize memory usage by choosing the appropriate data types. For instance, prefer `int8_t` for small integer values and avoid unnecessary allocations.

---

## **5. Naming Conventions**

- **Functions**: Use `camelCase`.
- **Variables**: Use `snake_case`.

- Function names should follow the **camelCase** convention for better readability and clarity.
  
  Example:

  ```c
  int calculateSum(int a, int b);

---

## **6. Variable Declarations**

- **Define all variables at the start of a function** with an ideally **1-line gap** between the code and the variable declarations.

Keeping all variables at the start of the function ensures clear and organized code. Use a 1-line gap for better visual separation between logic and declarations.

```c
void exampleFunction() {
    int a;
    float b;

    // Code logic starts here
    a = 10;
    b = 20.5f;
}
```

---

## **7. Ternary Operator Usage**

- **Prefer the ternary operator over `if-else`** statements, unless absolutely necessary.

The ternary operator is concise and often more readable for simple conditional assignments. Use `if-else` only for complex logic or when multiple statements are needed.

```c
// Preferred usage of ternary operator
int result = (x > y) ? x : y;
```

---

## **8. Code Blocks**

- **Always surround code blocks with curly braces `{}`**, even for single-line statements.

Using curly braces for all code blocks, regardless of the number of statements inside, prevents potential issues when modifying the code in the future. This practice improves readability and reduces errors.

```c
if (condition) {
    // Always use curly braces, even for one-line statements
    doSomething();
}
```

---

## **9. Pointer Variable Naming**

- **Use the `pVar` prefix** for variables that are pointers to regular variables.

Prefix pointer variables with `p` to clearly distinguish them from regular variables. This helps avoid confusion and improves code readability, especially in larger codebases.

```c
// Correct: Pointer variable with 'p' prefix
int *pValue;  // Pointer to an integer variable

// Incorrect: No clear indication that it's a pointer
int *value;  // Could be confusing without the 'p' prefix
```

---

## **10. Comments**

- **Exclude comments unless absolutely necessary.**

Focus on writing clear and self-explanatory code. Comments should be used sparingly and only when the code's intention is not immediately obvious. If a comment is necessary, ensure it explains why something is being done rather than what is being done.

### Best practices for comments

- Use comments to explain complex logic or algorithms that require additional context.
- Avoid stating the obvious or repeating what the code already expresses.
- Write comments in complete sentences with proper grammar for clarity.

```c
// Necessary comment explaining a non-trivial piece of logic
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

---

## **11. External Dependencies**

- **External dependencies must be placed in the `include` directory.**

All third-party libraries, header files, or other external dependencies should be copied into the `include/` directory. This practice helps keep the project organized by separating external resources from the core codebase. It ensures that all header files and libraries are centralized and easily accessible, preventing clutter in the source code directories.

### Directory structure example

Ensure that your project follows this structure:

```hooman
📦Project
┣ 📂include
┃ ┣ 📜external_lib1.h
┃ ┗ 📜external_lib2.h
┣ 📂src
┣ 📂docs
┗ 📜README.md
```

By placing external dependencies in the `include` folder, you maintain a clean separation of concerns, making the project easier to manage and scale.
