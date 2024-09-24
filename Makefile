# Compiler
CXX = g++

# Drapeaux de compilation (C++ 11 minimum)
CXXFLAGS = -std=c++11 -Wall

# Drapeaux pour les bibliothèques
LDFLAGS = -lglfw -lGLEW -lGL -ldl

# Dossiers
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Fichiers sources et objets
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Cible finale
EXEC = $(BINDIR)/humangl

# Commande par défaut
all: $(EXEC)

# Règle pour construire l'exécutable
$(EXEC): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Règle pour construire les fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o $@ -c $<

# Nettoyage des fichiers générés
clean:
	@rm -rf $(OBJDIR) $(BINDIR)

# Nettoyage complet
fclean: clean
	@rm -rf $(EXEC)

# Recompile tout
re: fclean all
