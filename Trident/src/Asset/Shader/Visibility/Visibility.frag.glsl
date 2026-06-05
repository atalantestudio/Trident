#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

in Interstage {
	layout(location = 0) flat uint visibility;
} interstage;

layout(location = 0) out uint visibility;

void main() {
	visibility = interstage.visibility;
}