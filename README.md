# Installation Engine
Simulation of an Installation Manager for Packages and Modules in C++

The system manages two types of components:
- Module: a basic installable component.
- Package: a component that can contain modules and/or other packages.

Each component can be in one of the following states:
- "PENDING"
- "INSTALLED"
- "FAILED"

The installation process supports dependency management, rollback on failure, and state-change notifications using the Observer pattern.

----------------------------------------------------------------

| Supported Commands |

Create components
  
- Create a module:

MODULE <component_id> <title>

Example:

MODULE M1 Database

- Create a package:

PACKAGE <component_id> <title>

Example:

PACKAGE P1 Backend

---

 Define Dependencies

- Add a component to a package:

ATTACH <package_id> <component_id>

Example:

DEPENDS P1 M1

A package may contain both modules and other packages.

---

Install

- Install a component:

INSTALL <component_id>

If the target is a package, all of its dependencies are installed recursively.

If any dependency fails, the installation is rolled back according to the project specification.

---

- Uninstall One Component
  
UNINSTALL <component_id>

Removes the specified component.

Dependencies that are no longer required by any installed package are also removed.

---

- Uninstall Everything

UNINSTALL -A

Uninstalls all installed components and returns the system to its initial state.

---

- Simulate Failure

  MOCK_FAIL <component_id>

Marks a component as a mocked failure component.

When an installation attempt reaches this component, the installation fails and the component enters the "FAILED" state.

Example:

MOCK_FAIL M1

INSTALL P1

If "P1" depends on "M1", installation fails when "M1" is processed.

---

 - Recover a Failed Component
  
RESOLVE <component_id>

Removes the mocked failure flag from a component.

The component can then be installed normally again.

Example:

FAIL M1
RESOLVE M1
INSTALL M1

----------------------------------------------------------------

Build

g++ -std=c++17 src/*.cpp -Iinclude -o installer

---

Run

Linux/macOS:

./installer < input.txt

Windows:

installer.exe < input.txt

---

Design Patterns

- Composite Pattern
- Observer Pattern
