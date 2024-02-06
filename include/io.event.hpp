/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.event.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/18 20:07:08 by diroyer           #+#    #+#             */
/*   Updated: 2024/02/04 13:24:31 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_EVENT_HPP
# define IO_EVENT_HPP

class IOEvent {
	public:
		inline virtual ~IOEvent(void) {} ;
		virtual void	read(void) = 0;
		virtual void	write(void) = 0;
		virtual int		fd(void) const = 0;
		virtual void	disconnect(void) = 0;
};

#endif