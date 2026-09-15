# Turbo

A Compiler and Byte Code Virtual Machine for a Pascal-like language

## Documentation

<h1>🚀 Turbo Pascal 64-bit Ecosystem</h1>

<blockquote>
    <strong>A modern tribute to a classic legend.</strong> An open-source, lightweight development ecosystem built in memory of <strong>Borland Turbo Pascal</strong> (1980s). This project modernizes classic Pascal syntax with a fully functional compiler and a robust <strong>64-bit Virtual Machine</strong> engineered natively for <strong>macOS</strong> and <strong>Linux</strong>.
</blockquote>

<hr>

## ⚡ Core Features

### 🛠️ The Compiler Subsystem
* **Pascal-Oriented Syntax:** Full support for custom Pascal-like semantics, driving lexical analysis, parsing, and code generation.
* **Dual-Mode Engine:** Flexible execution directly from raw **Source Text Files** (interpreted) or pre-compiled **Virtual Machine Binary Files**.
* **Type Safety Core:** Integrated validation subsystem enforcing strict runtime type checking.

### 🖥️ The Virtual Machine Subsystem
* **64-Bit Native Architecture:** Fully optimized for x86_64 and ARM64 registers and address spaces on Unix-like systems.
* **Smart Runtime Setup:** Instant bootstrapping of critical CPU-like pointers (`ip`, `dx`, `sp`) and runtime memory segments.
* **Dynamic Call Stack:** Native isolation of scopes and execution contexts using optimized Activation Records (`ActivationRecord`).
* **Low-Level Serialization:** High-performance binary object serialization and bulk data transfer routines.

<hr>

## 📁 Repository Structure

<table border="1" cellpadding="5" cellspacing="0" width="100%">
    <thead>
        <tr>
            <th>File</th>
            <th>Subsystem / Layer</th>
            <th>Responsibility</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><strong><code>turbo.c</code></strong></td>
            <td>Main / Core I/O</td>
            <td>App entry point, file persistence, and object serialization.</td>
        </tr>
        <tr>
            <td><strong><code>lexico.c</code></strong></td>
            <td>Compiler Frontend</td>
            <td>Lexical analyzer (tokenizer) for parsing Pascal source files.</td>
        </tr>
        <tr>
            <td><strong><code>sintactico.c</code></strong></td>
            <td>Compiler Frontend</td>
            <td>Syntactic parser. Builds the Abstract Syntax Tree (AST) &amp; checks types.</td>
        </tr>
        <tr>
            <td><strong><code>gencode.c</code></strong></td>
            <td>Compiler Backend</td>
            <td>Target code generation. Outputs custom virtual machine bytecode.</td>
        </tr>
        <tr>
            <td><strong><code>maquina.c</code></strong></td>
            <td>Virtual Machine</td>
            <td>Core VM engine. Runs the <em>Fetch-Decode-Execute</em> cycle and stack copies.</td>
        </tr>
        <tr>
            <td><strong><code>*.h</code> headers</strong></td>
            <td>Shared Architecture</td>
            <td>Structural definitions (<code>ActivationRecord</code>, registers, shared state).</td>
        </tr>
        <tr>
            <td><strong><code>Makefile</code></strong></td>
            <td>Build Automation</td>
            <td>Automated, warning-free build script optimized for GCC and Clang.</td>
        </tr>
    </tbody>
</table>

<hr>

## 🛠️ Requirements & Installation

This ecosystem compiles with **zero warnings** natively on modern 64-bit Unix platforms.

### 💻 OS Compatibility
* **macOS** *(Intel & Apple Silicon / M-Series via native ARM64 compilation)*
* **Linux** *(Ubuntu, Debian, Fedora, Arch, and derivatives)*

### ⚙️ Compilation Commands

To build the entire ecosystem into a single, clean executable named `turbo`:

<pre><code>make</code></pre>

To wipe intermediate object files (`*.o`) and clear binary artifacts:

<pre><code>make clean</code></pre>

<hr>

## 🚀 Quick Start Guide

Once the `turbo` binary is compiled, you can leverage its dual-mode engine straight from your terminal:

<pre><code># Mode 1: Execute directly from raw Pascal text (Interpreted Mode)
./turbo program.pas

# Mode 2: Execute a pre-compiled VM bytecode binary layout
./turbo --run program.bin</code></pre>

<hr>

## 🏗️ Execution Blueprint & Bytecode Mapping

The compiler features an execution map pipeline. Alongside your binary output, `turbo` automatically dumps an **Assembly-like Mapping File (`.mas`)** that visually breaks down stack pointer operations.

### 📝 Example: Recursive Factorial (`ej04.pas`)

<pre><code>var
    numero: Integer;

function Factorial(n: Integer): Integer;
begin
    if (n = 0) or (n = 1) then Factorial := 1
    else Factorial := n * Factorial(n - 1);
end;

begin
    numero := 5;
    WriteLn('El factorial de:', numero, ' es ', Factorial(numero));   
end.</code></pre>

### ⚙️ Output Blueprint State (`ej04.mas`)

<pre><code>pas/ej04.mas
	0000-0000 PSH _VARIABLE  	         Integer	0000:0000
	0000-0005 PSH _CONSTANTE	         Integer	0000:0011
	0000-000A EQC
	0000-0016 ORC
	0000-0017 IFC 0024
	0000-001A PSH _CONSTANTE	         Integer	0000:0015
	0000-001F SAV
	0000-0020 RTN
    [... VM Allocation Trace ...]
	0000-0034 ALC 0000:0004
	0000-0039 CPY 0001
	0000-003B CLL 0000:0000
	0000-0042 MUL
	0000-0044 RTN
0000-0082 FIN</code></pre>

<hr>

## 🤝 Contributing & Guidelines

Contributions extending the VM custom instruction set or refining AST parsing algorithms are highly appreciated. 

<blockquote>
    ⚠️ <strong>Memory Warning:</strong> Always validate dynamic memory allocations (<code>malloc</code> returns) inside your patches to prevent unexpected runtime segmentation faults on 64-bit systems.
</blockquote>

<hr>

*Built with passion for low-level software engineering, keeping the spirit of 80s programming alive on modern 64-bit Unix platforms.*

<hr>

## ✒️ Author

* **jmyix** - *Initial work & Architecture design* - [@jmyix](https://github.com)
