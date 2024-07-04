#include <stdio.h>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <math.h>

struct Todo
{
    int id;
    char text[100];
    bool done;
};

void makeTodoList(Todo *todos, int size)
{
    for (int i = 0; i < size; i++)
    {
        todos[i].id = i;
        sprintf(todos[i].text, "Todo item %d", i);
        todos[i].done = false;
    }
}

extern "C"
{

    EMSCRIPTEN_KEEPALIVE
    void printTodoList(Todo *todos, int size)
    {
        for (int i = 0; i < size; i++)
        {
            printf("Todo %d: %s, done: %d\n", todos[i].id, todos[i].text, todos[i].done);
        }
    }

    EMSCRIPTEN_KEEPALIVE
    EM_JS(void, markItemNotDone, (int id), {
        const oneItems = document.querySelectorAll(".todo-container__list--done .todo-container__list-item");
        const ulDone = document.querySelector(".todo-container__list--todo");
        const todoItem = Array.from(oneItems).find(function(item) {
            return item.dataset.id == id.toString();
        });
        if (todoItem)
        {
            const btn = todoItem.querySelectorAll("button")[1];
            btn.innerHTML = "Mark Done";
            btn.onclick = function()
            {
                markItemDone(id);
            };
            ulDone.appendChild(todoItem);
        }
    });

    EMSCRIPTEN_KEEPALIVE
    EM_JS(void, markItemDone, (int id), {
        const todoItems = document.querySelectorAll(".todo-container__list--todo .todo-container__list-item");
        const ulDone = document.querySelector(".todo-container__list--done");
        const todoItem = Array.from(todoItems).find(function(item) {
            return item.dataset.id == id.toString();
        });
        if (todoItem)
        {
            const btn = todoItem.querySelectorAll("button")[1];
            btn.innerHTML = "Not Done";
            btn.onclick = function()
            {
                markItemNotDone(id);
            };
            ulDone.appendChild(todoItem);
        }
    });

    EMSCRIPTEN_KEEPALIVE
    EM_JS(void, renderTodoList, (Todo * todos, int size), {
        const ulTodo = document.querySelector(".todo-container__list--todo");
        const ulDone = document.querySelector(".todo-container__list--done");
        const todoSize = 108;
        const todoArray = [];
        for (let i = 0; i < size; i++)
        {
            const baseOffset = todos + i * todoSize;
            const todo = {
                id : getValue(baseOffset, 'i32'),
                text : UTF8ToString(baseOffset + 4),
                done : getValue(baseOffset + 104, 'i8') != 0,
            };
            todoArray.push(todo);
        }

        ulTodo.innerHTML = ""; // Clear existing items
        ulDone.innerHTML = ""; // Clear existing items

        todoArray.forEach(function(todo) {
            const li = document.createElement("li");
            li.className = "todo-container__list-item";
            li.setAttribute("data-id", todo.id);
            li.innerHTML = "<span>#" + todo.id + ": " + todo.text + "</span>";

            const deleteButton = document.createElement("button");
            deleteButton.style.margin = "0px 10px";
            deleteButton.textContent = "Delete";
            deleteButton.onclick = function()
            {
                // Implement delete functionality here
                li.remove();
            };

            const markDoneButton = document.createElement("button");
            markDoneButton.textContent = "Mark Done";
            markDoneButton.onclick = function()
            {
                markItemDone(todo.id);
            };

            li.appendChild(deleteButton);
            li.appendChild(markDoneButton);
            ulTodo.appendChild(li);
        });
    });
}

EMSCRIPTEN_KEEPALIVE
int main()
{
    Todo todos[5];
    makeTodoList(todos, 5);
    printf("Hello, World!\n");
    printTodoList(todos, 5);
    renderTodoList(todos, 5);
    return 0;
}
