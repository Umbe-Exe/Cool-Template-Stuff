#pragma once
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

template<class F, class... Args>
struct Command {
	using Tuple = std::tuple<std::decay_t<F>, std::decay_t<Args>...>;

	constexpr Command(F &&f, Args... args) {
		arguments = Tuple(std::forward<F>(f), std::forward<Args>(args)...);
		invoker   = get_invoke<Tuple>(std::make_index_sequence<1 + sizeof...(Args)>{});
	}

	void constexpr execute() {
		invoker(arguments);
	}

	static void executor(Command *command) {
		command->execute();
	}

private:
	Tuple arguments;
	void (*invoker)(Tuple&);

	template <class Tuple, size_t... Indices>
	static constexpr auto get_invoke(std::index_sequence<Indices...>) {
		return &invoke<Tuple&, Indices...>;
	}

	template <class Tuple, size_t... Indices>
	static constexpr void invoke(Tuple &tuple) {
		std::invoke(std::move(std::get<Indices>(tuple))...);
	}
};

struct CommandPool {

	template<class F, class... Args>
	void record(Command<F, Args...> &&command) {
		static std::vector<Command<F, Args...>> comms;
		comms.push_back(command);

		commands.push_back(std::make_pair((void (*)(void *))&Command<F, Args...>::executor, (void *)&comms.back()));
	}

	void inline execute() {
		for(auto &info : commands) info.first(info.second);
	}

private:
	std::vector<std::pair<void (*)(void *), void *>> commands;
};