CXXFLAGS = -I SFML/include
LDFLAGS = -L SFML/lib -lsfml-graphics -lsfml-window -lsfml-system

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SOURCES:$(SRCDIR)/%.cpp=$(SRCDIR)/%.o)

all: game

game: $(OBJS)
	@echo "** Сборка игры"
	g++ -o game $(OBJS) $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	g++ $(CXXFLAGS) -c $< -o $@

clean:
	@echo "** Очистка файлов..."
	rm -f game $(OBJS)
	@echo "** Очистка завершена."